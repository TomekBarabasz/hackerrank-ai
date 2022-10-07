//#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <immintrin.h>
#include "click-o-mania.h"

using namespace std;

namespace Click_o_mania
{
template <typename T>
void initGrid(T init,SearchState *pss)
{
    auto *grid = pss->grid;
    int idx=0;
    for( auto &row : init ) {
        for (auto c : row) {
            grid[idx++] = c;
        }
    }
}
template <typename T>
SearchState* makeGrid_i(T init)
{
    static_assert(std::is_same<std::string::value_type,char>::value);
    const int nrow = init.size();
    const int ncol = init.begin()->size();
    auto *pss = SearchState::alloc(nrow,ncol);
    initGrid(init,pss);
    return pss;
}
inline __m256i _mm256_i32gather_epi8 (int const* base_addr, __m256i vindex)
{
    __m256i v32index = _mm256_srli_epi32(vindex,2);
    __m256i shift = _mm256_slli_epi32(_mm256_and_si256(vindex, _mm256_set1_epi32(3)),3);
    __m256i values = _mm256_i32gather_epi32(base_addr, v32index, 4);
    values = _mm256_srlv_epi32(values, shift);
    return  _mm256_and_si256(values,_mm256_set1_epi32(0xff));
}
inline std::tuple<__m256i,__m256i> _mm256_i32gather_2xepi8 (int const* base_addr_1, int const* base_addr_2, __m256i vindex)
{
    __m256i v32index = _mm256_srli_epi32(vindex,2);
    __m256i shift = _mm256_slli_epi32(_mm256_and_si256(vindex, _mm256_set1_epi32(3)),3);
    __m256i values_1 = _mm256_i32gather_epi32(base_addr_1, v32index, 4);
    __m256i values_2 = _mm256_i32gather_epi32(base_addr_2, v32index, 4);
    values_1 = _mm256_srlv_epi32(values_1, shift);
    values_2 = _mm256_srlv_epi32(values_2, shift);
    values_1 = _mm256_and_si256(values_1,_mm256_set1_epi32(0xff));
    values_2 = _mm256_and_si256(values_2,_mm256_set1_epi32(0xff));
    return {values_1,values_2};
}

template<typename T>
T _mm_load(void*ptr) { return _mm256_load_si256(reinterpret_cast<__m256i const*>(ptr)); }
template<typename T>
void _mm_store(const T & val, void*ptr) { _mm256_store_si256(reinterpret_cast<__m256i*>(ptr),val); }
inline __m256i _mm_load(const Point * pt)
{
    return _mm256_set1_epi32(*reinterpret_cast<const int*>(pt));
}
inline __m256i _mm_load(const Point & pt1, const Point & pt2)
{
    return _mm256_permute2f128_si256(_mm256_set1_epi32(*reinterpret_cast<const int*>(&pt1)), _mm256_set1_epi32(*reinterpret_cast<const int*>(&pt2)),0x21);
}
inline void copyTo(const __m256i& src, void *dst) { _mm256_store_si256(( __m256i*)dst, src); }
template <typename T, typename U=int32_t>
void print(const char* title, const T & val)
{
    static ALIGNED(64) U output[sizeof(T)/sizeof(U)] = {0};
    copyTo(val, output);
    printf("%s : ", title);
    const char *fmt = std::is_signed<U>::value ? "%d " : "0x%x ";
    for (auto a : output) printf(fmt, a);
    printf("\n");
}

/* NOTE: 
 * dim is {row-1,col-1}
 * input is {row,col}
 * return points {{row,col} x8}, valid
 */

inline std::tuple<__m256i,__m256i> generateNeighbours(Point pts[2],__m256i dim,__m256i dirs)
{
    auto rc = _mm_load(pts[0], pts[1]);
    rc = _mm256_add_epi16(rc,dirs);
    auto valid = _mm256_or_si256(_mm256_cmpgt_epi16(_mm256_setzero_si256(),rc), _mm256_cmpgt_epi16(rc,dim));
    valid = _mm256_cmpeq_epi32(valid, _mm256_setzero_si256());
    return {rc,valid};
}

struct Scratchpad
{
    __m256i dim;
    __m256i dirs;
    int32_t pt_valid[8], pt_idx[8], grid_upd[8];
    Point pt_rc[8];
};

inline void process2points(Point inp[2], int color, int group_id, uint32_t * grid_group, Scratchpad& wrk)
{
    auto [rc,valid] = generateNeighbours(inp,wrk.dim,wrk.dirs);
    auto r = _mm256_blend_epi16(rc,_mm256_setzero_si256(),0b10101010);
    auto c = _mm256_srli_epi32(_mm256_blend_epi16(rc,_mm256_setzero_si256(),0b01010101),16);
    auto ncols = _mm256_add_epi32(_mm256_srli_epi32(wrk.dim,16),_mm256_set1_epi32(1));
    auto index = _mm256_add_epi32(c, _mm256_mullo_epi32(r, ncols));
    index = _mm256_and_si256(index,valid);

    //color is lower 16 bit
    //group is upper 16 bit
    auto colors = _mm256_i32gather_epi32(reinterpret_cast<const int*>(grid_group), index, 4);
    auto groups = _mm256_srli_epi32(colors,16);
    colors = _mm256_and_si256(colors,_mm256_set1_epi32(0xffff));

    auto group_upd = _mm256_slli_epi32(_mm256_set1_epi32(group_id), 16);

    valid = _mm256_and_si256(valid, _mm256_cmpeq_epi32(colors,_mm256_set1_epi32(color)));
    valid = _mm256_and_si256(valid, _mm256_cmpeq_epi32(groups,_mm256_setzero_si256()));

    group_upd = _mm256_or_si256(_mm256_and_si256(group_upd,valid),colors);

    _mm256_store_si256((__m256i*)wrk.pt_valid,valid);
    _mm256_store_si256((__m256i*)wrk.pt_rc,rc);
    _mm256_store_si256((__m256i*)wrk.pt_idx,index);
    _mm256_store_si256((__m256i*)wrk.grid_upd,group_upd);
}

//NOTE: dim = {nrow-1,ncol-1}
inline void fillGroupFromPositionAVX(SearchState& ss, Point & dim, Point & seed, int color,int group_id, Workspace& wrk)
{
    auto & ring = wrk.ring;
    Point2x pts;
    Point dummy={-2,-2};
    int npts=0;
    ALIGNED(32) Scratchpad scr;
    scr.dim = _mm_load(&dim);
    scr.dirs = _mm256_setr_epi16(-1,0,0,-1,0,1,1,0,   -1,0,0,-1,0,1,1,0);

    ring.push_back( Point2x{{seed,dummy}} );
    ss.setGroupAt(seed.r,seed.c,group_id);

    while (!ring.empty())
    {
        auto pt2 = ring.popfirst();
        process2points(pt2.arr, color, group_id, ss.grid, scr);
        for(int i=0;i<8;++i)
            if (scr.pt_valid[i])
            {
                pts.arr[npts++] = scr.pt_rc[i];
                ss.grid[scr.pt_idx[i]] = scr.grid_upd[i];
                if (2==npts) {
                    ring.push_back(pts);
                    npts = 0;
                }
            }
        if (ring.empty() && 1==npts){
            pts.arr[1] = dummy;
            ring.push_back(pts);
            npts = 0;
        }
    }
}

inline void fillGroupFromPosition(SearchState& ss, Point & dim, Point & seed, int color,int group_id, Workspace& wrk)
{
    static constexpr std::tuple<int8_t,int8_t> Directions[4] = {{-1,0},{0,-1},{0,1},{1,0}};
    auto & points = wrk.ring;
    Point2x p2x = {{seed}};
    points.push_back( p2x );
    while (!points.empty())
    {
        p2x = points.popfirst();
        auto r = p2x.arr[0].r;
        auto c = p2x.arr[0].c;
        ss.setGroupAt(r,c,group_id);
        
        #pragma GCC unroll 4
        for (auto [dr,dc]:Directions)
        {
            const int8_t r1=r+dr;
            if (r1 < 0 || r1 > dim.r) continue;
            const int8_t c1=c+dc;
            if (c1 < 0 || c1 > dim.c) continue;
            const auto [c,gr] = ss.getAt(r1,c1);
            if (color == c && 0 == gr)
            {
                p2x.arr[0] = {r1,c1};
                points.push_back( p2x );
            }
        }
    }
}
int partitionGrid(SearchState& ss,Workspace& wrk)
{
    const int nrow = ss.nrow;
    const int ncol = ss.ncol;
    uint8_t nextGroup = 1;
    Point seed, dim={ss.nrow-1,ss.ncol-1};
    int idx = 0;
    for (seed.r = 0; seed.r < nrow; ++seed.r)
        for(seed.c = 0; seed.c < ncol; ++seed.c,++idx)
        {
            const auto [color,group] = ss.getAt(idx);
            if (EmptySpace != color && 0 == group)
            {
                fillGroupFromPosition(ss,dim,seed,color,nextGroup++,wrk);
            }
        }
    return nextGroup-1;
}
void collectGroups(SearchState& ss,int ngroups,Workspace& wrk, BlockList& blocks)
{
    blocks.ngroups = ngroups;
    blocks.npoints = 0;
    const int totSize = ss.nrow*ss.ncol;
    const size_t group_count_bytes = sizeof(Workspace::GroupCount_t)*(ngroups+1);
    auto* group_count = wrk.group_count;
    memset(group_count,0,group_count_bytes);
    for(int i=0;i<totSize;++i){
        ++group_count[ ss.groupAt(i) ];
    }
    group_count[0] = 0; //group id 0 is invalid
    for(int i=1;i<=ngroups;++i)
    {
        const int first = group_count[i-1];
        const int nelem = group_count[i];
        group_count[i] += group_count[i-1];
        blocks.groups[i-1] = {i,first,nelem};
    }
    memset(group_count,0,group_count_bytes);
    int idx = 0;
    for (int16_t r = 0; r < ss.nrow; ++r)
        for (int16_t c = 0; c < ss.ncol; ++c,++idx)
        {
            auto group_id = ss.groupAt(idx);
            if (group_id != 0) 
            {
                const auto group_idx = group_id - 1;
                const int pt_idx = get<1>(blocks.groups[group_idx]) + group_count[group_idx];
                blocks.points[pt_idx] = Point{r,c};
                ++group_count[group_idx];
            }
        }
    sortGroups(blocks);
}
void sortGroups(BlockList& blocks)
{
    auto *begin = blocks.groups;
    auto *end = blocks.groups + blocks.ngroups;
    using Group_t = BlockList::Group_t;
    std::sort(begin,end,[](const Group_t&a, const Group_t&b){ return get<2>(a) > get<2>(b);});
}
SearchState* removeGroup(SearchState& ss, int group_idx, Workspace& wrk)
{
    auto * newss = wrk.allocSearchState();
    newss->nrow = ss.nrow;
    newss->ncol = ss.ncol;
    const auto totSize = ss.nrow*ss.ncol;
    for (int i=0;i<totSize; ++i) newss->grid[i] = ss.gridAt(i);
    const auto [id,first,nelem] = ss.blocks.groups[group_idx];
    auto * newgrid = newss->grid;
    for (int i=0;i<nelem;++i) {
        const auto [r,c] = ss.blocks.points[first + i];
        newgrid[r*ss.ncol+c] = EmptySpace;
    }
    for (int c=0;c<newss->ncol;)
    {
        const bool empty = updateColumn(*newss,c);
        if (empty)
        {
            removeColumn(*newss,c);
        }else{
            ++c;
        }
    }
    removeEmptyRows(*newss);

    return newss;
}
bool updateColumn(SearchState& ss,int icol)
{
    int dest_r = (ss.nrow-1)*ss.ncol+icol,src_r=-1;
    auto * grid = ss.grid;
    
    Next:
    for(;dest_r >= ss.ncol; dest_r -= ss.ncol)
        if (grid[dest_r] == EmptySpace)
        {
            if (src_r < 0) src_r = dest_r - ss.ncol;
            for(; src_r >= 0; src_r -= ss.ncol)
                if (grid[src_r] != EmptySpace)
                {
                    grid[dest_r] = grid[src_r];
                    grid[src_r] = EmptySpace;
                    dest_r -= ss.ncol;
                    src_r -= ss.ncol;
                    goto Next;
                }
            goto Exit;
        }
    
    Exit:
    return grid[ (ss.nrow-1)*ss.ncol + icol] == EmptySpace;
}
void removeColumn(SearchState& ss, int icol)
{
    const int ElemSize = sizeof(ss.grid[0]);
    auto *dst = ss.grid + icol, *src = dst + 1;
    int cnt = ss.ncol - 1;
    for (int r=0;r<ss.nrow-1;++r)
    {
        memmove(dst,src,cnt * ElemSize);
        dst += cnt;
        src += cnt+1;
    }
    //last row
    memmove(dst,src,(ss.ncol-1-icol) * ElemSize);
    --ss.ncol;
}
void removeEmptyRows(SearchState& ss)
{
    auto *pr = ss.grid;
    int r;
    for(r=0;r<ss.nrow; ++r)
        for(int c=0;c<ss.ncol;++c)
            if (*pr++ != EmptySpace)
                goto Exit;
    Exit:
    if (r > 0)
    {
        memmove(ss.grid,ss.grid+r*(ss.ncol),(ss.nrow-r)*ss.ncol*sizeof(ss.grid[0]));
        ss.nrow -= r;
    }
}
bool stopConditionReached(SearchState& ss)
{
    for (int gi=0;gi<ss.blocks.ngroups;++gi)
        if (get<2>(ss.blocks.groups[gi]) > 1)
            return false;
    return true;
}
void printGrid(const SearchState& ss)
{
    auto * grid = ss.grid;
    for(int r=0;r<ss.nrow;++r) {
        for (int c=0;c<ss.ncol;++c) printf("%c",*grid++);
        printf("\n");
    }
}
//return [points, group_index]
std::tuple<int,int> solveInternal(SearchState& ss,Workspace& wrk,int level)
{
    const int ngroups = partitionGrid(ss,wrk);
    //printf("level %d grid (%d,%d) groups %d\n",level,ss.nrow,ss.ncol,ngroups);
    collectGroups(ss,ngroups,wrk,ss.blocks);
    if (stopConditionReached(ss))
    {
        static unsigned int nLeafs = 0;
        if (++nLeafs > 1000000) exit(1);
        printf("\rlevel %d pts %d",level,ss.blocks.ngroups);
        return {ss.blocks.ngroups,0};
    }
    else
    {
        int min_gi = -1;
        int min_pts = 255;
        for (int gi=0;gi<ss.blocks.ngroups;++gi)
        {
            auto [id,first,nelem] = ss.blocks.groups[gi];
            if (nelem > 1)
            {
                auto *nss = removeGroup(ss,gi,wrk);
                auto [pts,x] = solveInternal(*nss,wrk,level+1);
                wrk.releaseSearchState(nss);
                if (pts < min_pts)
                {
                    if (0==pts) [[unlikely]]
                    {
                        return {0,gi};
                    } else {
                        min_pts = pts;
                        min_gi = gi;
                    }
                }
            }
        }
        return {min_pts,min_gi};
    }
}
template<typename T>
Point solve_i(T init)
{
    Workspace wrk = Workspace(init.size(),init.begin()->size());
    auto *pss = wrk.allocSearchState();
    initGrid(init,pss);
    auto [points,group_idx] = solveInternal(*pss,wrk,1);
    auto res = pss->blocks.points[ get<1>(pss->blocks.groups[group_idx]) ];
    wrk.releaseSearchState(pss);
    return res;
}

//template decltype(solve<std::initializer_list<std::string>>) solve<std::initializer_list<std::string>>;
//template decltype(solve<const std::vector<std::string>&>) solve<const std::vector<std::string>&>;
//template Point solve(const std::vector<string>&);
Point solve(const std::vector<std::string> & init) { return solve_i(init); }
Point solve(std::initializer_list<std::string> init) { return solve_i(init); }

SearchState* makeGrid(const std::vector<string>& init) { return makeGrid_i(init);}
SearchState* makeGrid(std::initializer_list<string> init) { return makeGrid_i(init);}
}