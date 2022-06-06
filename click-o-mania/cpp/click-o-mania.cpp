//#include <iostream>
#include <string>
#include <cstring>
#include "click-o-mania.h"

using namespace std;

namespace Click_o_mania
{
void initGrid(std::initializer_list<std::string> init,SearchState *pss)
{
    auto *grid = pss->grid;
    for( auto &row:init ) {
        memcpy(grid,row.c_str(),row.size());
        grid += row.size();
    }
}
SearchState* makeGrid(std::initializer_list<std::string> init)
{
    static_assert(std::is_same<std::string::value_type,char>::value);
    const int nrow = init.size();
    const int ncol = init.begin()->size();
    auto *pss = SearchState::alloc(nrow,ncol);
    initGrid(init,pss);
    return pss;
}
inline void fillGroupFromPosition(const SearchState& ss, int r0, int c0, uint8_t color,uint8_t group, Workspace& wrk)
{
    static constexpr std::tuple<int8_t,int8_t> Directions[4] = {{-1,0},{0,-1},{0,1},{1,0}};
    auto & points = wrk.ring;
    points.push( {r0,c0} );
    while (!points.empty())
    {
        auto [r,c] = points.popfirst();
        wrk.group_map[r*ss.ncol+c] = group;
        #pragma GCC unroll 4
        for (auto [dr,dc]:Directions)
        {
            const int8_t r1=r+dr;
            if (r1 < 0 || r1 >= ss.nrow) continue;
            const int8_t c1=c+dc;
            if (c1 < 0 || c1 >= ss.ncol) continue;
            const auto idx = r1*ss.ncol+c1;
            if (color == ss.grid[idx] && 0==wrk.group_map[idx])
            {
                points.push( {r1,c1} );
            }
        }
    }
}
int partitionGrid(const SearchState& ss,Workspace& wrk)
{
    const int nrow = ss.nrow;
    const int ncol = ss.ncol;
    uint8_t nextGroup = 1;
    memset(wrk.group_map,0,nrow*ncol);
    auto *group_map = wrk.group_map;
    auto *grid = ss.grid;
    for (int r=0;r<nrow;++r)
        for(int c=0;c<ncol;++c)
        {
            if (0==*group_map && EmptySpace != *grid)
            {
                fillGroupFromPosition(ss,r,c,*grid,nextGroup++,wrk);
            }
            ++group_map;
            ++grid;
        }
    return nextGroup-1;
}
void collectGroups(int nrow,int ncol,int ngroups,Workspace& wrk, BlockList<int8_t>& blocks)
{
    blocks.ngroups = ngroups;
    blocks.npoints = 0;
    const int totSize = nrow*ncol;
    const auto *group_map = wrk.group_map;
    int16_t* group_count = wrk.group_count;
    memset(group_count,0,sizeof(int16_t)*(ngroups+1));
    for(int i=0;i<totSize;++i){
        ++group_count[ group_map[i] ];
    }
    group_count[0] = 0; //group id 0 is invalid
    for(int i=1;i<=ngroups;++i)
    {
        const int first = group_count[i-1];
        const int nelem = group_count[i];
        group_count[i] += group_count[i-1];
        blocks.groups[i-1] = {i,first,nelem};
    }
    memset(group_count,0,sizeof(int16_t)*(ngroups+1));
    group_map = wrk.group_map;
    for (int r=0;r<nrow;++r)
        for (int c=0;c<ncol;++c,++group_map)
        {
            auto group_id = *group_map;
            if (*group_map != 0) 
            {
                const auto group_idx = *group_map-1;
                const int pt_idx = get<1>(blocks.groups[group_idx]) + group_count[group_idx];
                blocks.points[pt_idx] = {r,c};
                ++group_count[group_idx];
            }
        }
    sortGroups(blocks);
}
void sortGroups(BlockList<int8_t>& blocks)
{
    auto *begin = blocks.groups;
    auto *end = blocks.groups + blocks.ngroups;
    using Group = BlockList<int8_t>::Group;
    std::sort(begin,end,[](const Group&a, const Group&b){ return get<2>(a) > get<2>(b);});
}
SearchState* removeGroup(SearchState& ss, int group_idx, Workspace& wrk)
{
    auto * newss = wrk.allocSearchState();
    newss->nrow = ss.nrow;
    newss->ncol = ss.ncol;
    const auto totSize = ss.nrow*ss.ncol;
    memcpy(newss->grid,ss.grid,totSize*sizeof(ss.grid[0]));
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
    auto *dst = ss.grid + icol, *src = dst + 1;
    int cnt = ss.ncol - 1;
    for (int r=0;r<ss.nrow-1;++r)
    {
        memmove(dst,src,cnt*sizeof(ss.grid[0]));
        dst += cnt;
        src += cnt+1;
    }
    //last row
    memmove(dst,src,ss.ncol-1-icol);
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
    collectGroups(ss.nrow,ss.ncol,ngroups,wrk,ss.blocks);
    if (stopConditionReached(ss))
    {
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
std::tuple<int8_t,int8_t> solve(std::initializer_list<std::string> init)
{
    Workspace wrk = Workspace(init.size(),init.begin()->size());
    auto *pss = wrk.allocSearchState();
    initGrid(init,pss);
    auto [points,group_idx] = solveInternal(*pss,wrk,1);
    auto res = pss->blocks.points[ get<1>(pss->blocks.groups[group_idx]) ];
    wrk.releaseSearchState(pss);
    return res;
}
}