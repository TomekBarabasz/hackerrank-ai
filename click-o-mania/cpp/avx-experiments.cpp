#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <stdint.h>
#include <immintrin.h>
#include <gtest/gtest.h>
#include <tuple>

#include "click-o-mania.h"

using namespace std;

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
#if 0
template<typename U>
int extract(const __m256i & val, const int index);
template<>
int extract<int16_t>(const __m256i & val, int index) 
{   
    if (index > 4) {
        index /= 2;
        auto shifted = _mm256_slli_si256(val, index*16);
        return _mm256_extract_epi16(val,8);
    }else{
        auto shifted = _mm256_slli_si256(val, index*16);
        return _mm256_extract_epi16(val,0);
    }
}
template<>
int extract<int32_t>(const __m256i & val, int index) 
{   
    if (index > 2) {
        index /= 2;
        auto shifted = _mm256_slli_si256(val, index*32);
        return _mm256_extract_epi16(val,4);
    }else{
        auto shifted = _mm256_slli_si256(val, index*32);
        return _mm256_extract_epi16(val,0);
    }
}
template <typename T, typename U=int32_t>
void printe(const char* title, const T & val)
{
    printf("%s : ", title);
    const char *fmt = std::is_signed<U>::value ? "%d " : "0x%x ";
    const int nelem = 256 / 8/ sizeof(U);
    for (int i=0;i<nelem;++i) 
        printf(fmt, extract<U>(val,i));

    printf("\n");
}
#endif
template<typename T>
void printArr(const char* title,const T *ptr, size_t size)
{
    const char *fmt = std::is_signed<T>::value ? "%d " : "0x%x ";
    printf("%s : ",title);
    for (int i=0;i<size;++i)
        printf(fmt,ptr[i]);
    printf("\n");
}
namespace Click_o_mania
{

inline __m256i _mm256_i32gather_epi8 (int const* base_addr, __m256i vindex)
{
    __m256i v32index = _mm256_srli_epi32(vindex,2);
    __m256i shift = _mm256_slli_epi32(_mm256_and_si256(vindex, _mm256_set1_epi32(3)),3);
    __m256i values = _mm256_i32gather_epi32(base_addr, v32index, 4);
    values = _mm256_srlv_epi32(values, shift);
    return  _mm256_and_si256(values,_mm256_set1_epi32(0xff));
}

template<typename T>
T _mm_load(void*ptr) { return _mm256_load_si256(reinterpret_cast<__m256i const*>(ptr)); }
template<typename T>
void _mm_store(const T & val, void*ptr) { _mm256_store_si256(reinterpret_cast<__m256i*>(ptr),val); }

inline __m256i _mm_load(const Point * pt)
{
    auto mmPt = _mm256_set1_epi32(*reinterpret_cast<const int*>(pt));
    return _mm256_or_si256( _mm256_srli_epi32(mmPt,16), _mm256_slli_epi32(mmPt,16) );
}

inline __m256i _mm_load(const Point & pt1, const Point & pt2)
{
    auto pts = _mm256_permute2f128_si256(_mm256_set1_epi32(*reinterpret_cast<const int*>(&pt1)), _mm256_set1_epi32(*reinterpret_cast<const int*>(&pt2)),0x21);
    return _mm256_or_si256( _mm256_srli_epi32(pts,16), _mm256_slli_epi32(pts,16) );
}

/* NOTE: 
 * dim is {row-1,col-1}
 * input is {row,col}
 * return points {{row,col} x8}, valid
 */

inline std::tuple<__m256i,__m256i> generateNeighbours(Point pts[2],__m256i dim,__m256i dirs)
{
    auto rc = _mm_load(pts[0], pts[1]);
    //print<__m256i,int16_t>("rc",rc);
    //print<__m256i,int16_t>("dirs",dirs);
    rc = _mm256_add_epi16(rc,dirs);
    //print<__m256i,int16_t>("newrc",rc);
    //print<__m256i,int16_t>("dim",dim);
    //a>b?0xFFFF:0
    //0xFFFF if coord is invalid
    auto valid = _mm256_or_si256(_mm256_cmpgt_epi16(_mm256_setzero_si256(),rc), _mm256_cmpgt_epi16(rc,dim));
    valid = _mm256_cmpeq_epi32(valid, _mm256_setzero_si256());
    return {rc,valid};
}

struct Scratchpad
{
    __m256i dim;
    __m256i dirs;
    int32_t pt_idx[8], pt_valid[8];
    Point pt_rc[8];
};

inline void process2points(Point inp[2], int8_t color, const uint8_t * grid, uint8_t * group_map, Scratchpad& wrk)
{
    auto [rc,valid] = generateNeighbours(inp,wrk.dim,wrk.dirs);
    auto r = _mm256_blend_epi16(rc,_mm256_setzero_si256(),0b10101010);
    auto c = _mm256_srli_epi32(_mm256_blend_epi16(rc,_mm256_setzero_si256(),0b01010101),16);
    auto ncols = _mm256_add_epi32(_mm256_srli_epi32(wrk.dim,16),_mm256_set1_epi32(1));
    auto idx = _mm256_add_epi32(c, _mm256_mullo_epi32(r, ncols));
    idx = _mm256_and_si256(idx,valid);
    //print<__m256i,int16_t>("rc",rc);
    //print<__m256i,int32_t>("idx",idx);

    auto colors = _mm256_i32gather_epi8(reinterpret_cast<const int*>(grid),idx);
    auto groups = _mm256_i32gather_epi8(reinterpret_cast<const int*>(group_map),idx);
    valid = _mm256_and_si256(valid, _mm256_cmpeq_epi32(colors,_mm256_set1_epi32(color)));
    valid = _mm256_and_si256(valid, _mm256_cmpeq_epi32(groups,_mm256_setzero_si256()));

    _mm256_store_si256((__m256i*)wrk.pt_idx,idx);
    _mm256_store_si256((__m256i*)wrk.pt_valid,valid);
    _mm256_store_si256((__m256i*)wrk.pt_rc,rc);
}

//NOTE: dim = {nrow-1,ncol-1}
inline void fillGroupFromPositionAVX(const SearchState& ss, Point & dim, Point & seed, uint8_t color,uint8_t group, Workspace& wrk)
{
    auto & ring = wrk.ring;
    //vector<Point2x> ring;
    Point2x pts;
    Point dummy={-2,-2};
    int npts=0;
    ALIGNED(32) Scratchpad scr;
    scr.dim = _mm_load(&dim);
    scr.dirs = _mm256_setr_epi16(-1,0,0,-1,0,1,1,0,   -1,0,0,-1,0,1,1,0);

    ring.push_back( Point2x{{seed,dummy}} );
    wrk.group_map[ seed.r*ss.ncol+seed.c ] = group;
    while (!ring.empty())
    {
        auto pt2 = ring.popfirst();
        process2points(pt2.arr, color, ss.grid, wrk.group_map, scr);
        for(int i=0;i<8;++i)
            if (scr.pt_valid[i])
            {
                pts.arr[npts++]=scr.pt_rc[i];
                wrk.group_map[ scr.pt_idx[i] ] = group;
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

auto gatherValidPoints(Scratchpad& outp)
{
    std::vector<Point> queue;
    for (int i=0;i<8;++i)
        if (outp.pt_valid[i])
        {
            queue.push_back( outp.pt_rc[i] );
        }
    return queue;
}

Point operator+(const Point& a, const Point& b)
{
    return {a.r+b.r,a.c+b.c};
}
Point swap(const Point & p)
{
    return {p.c,p.r};
}
}
using namespace Click_o_mania;
template<typename T,int alignement>
T* makeGridArray(std::initializer_list<std::string> init)
{
    static_assert(std::is_same<std::string::value_type,char>::value);
    const int nrow = init.size();
    const int ncol = init.begin()->size();
    T * grid = reinterpret_cast<T*>(aligned_alloc(nrow*ncol,alignement));
    auto *grow = grid;
    for( auto &row:init ) {
        memcpy(grow,row.c_str(),row.size());
        grow += row.size();
    }
    return grid;
}

TEST(AVX, test_alloc)
{
    auto * grid = makeGridArray<int8_t,32>({"11122",
                                            "31344",
                                            "33354",
                                            "67855",
                                            "68795"});
    free(grid);
}
TEST(AVX, test_i32gather_epi8)
{
    static ALIGNED(32) int8_t data[] = {0,1,2,3,4,5,6,7};
    __m256i vidx = _mm256_setr_epi32(0,1,2,3,4,5,6,7);
    __m256i loaded = _mm256_i32gather_epi8( reinterpret_cast<const int*>(data),vidx);
    static ALIGNED(32) int32_t result[8] = {};
    _mm256_store_si256( reinterpret_cast<__m256i*>(&result),loaded);
    //print("loaded",loaded);
    for (int i=0;i<8;++i) {
        ASSERT_EQ(data[i], result[i]);
    }
}
TEST(AVX, test_coords_valid_nonneg)
{
    Point input[] = {{0,0},{1,1}};
    Point dim = {9,9};
    auto mmDirs = _mm256_setr_epi16(-1,0,0,-1,0,1,1,0,   -1,0,0,-1,0,1,1,0);

    auto [mm_rc, mm_valid] = generateNeighbours(input, _mm_load(&dim), mmDirs);
    ALIGNED(32) Point rc[8];
    ALIGNED(32) int32_t valid[8];
    _mm_store(mm_rc,rc);
    _mm_store(mm_valid,valid);

    const Point dirs[]  = {{-1,0},{0,-1},{0,1},{1,0}};

    Point & p0 = input[0];
    for (int i=0;i<4;++i)
    {
        auto p = p0 + dirs[i];
        //printf("i %d exp (%d,%d) ret (%d,%d) valid %x\n", i, p.r, p.c, rc[i].r, rc[i].c, valid[i]);
        ASSERT_EQ(p,rc[i]);
        
        const int32_t coords_valid = p.r >=0 && p.c >=0 ? -1 : 0;
        ASSERT_EQ(coords_valid,valid[i]);
    }

    Point & p1 = input[1];
    for (int i=0;i<4;++i)
    {
        auto p = p1 + dirs[i];
        //printf("i %d exp (%d,%d) ret (%d,%d) valid %x\n", i, p.r, p.c, rc[i+4].r, rc[i+4].c, valid[i+4]);
        ASSERT_EQ(p,rc[i+4]);

        const int32_t coords_valid = p.r >=0 && p.c >=0 ? -1 : 0;
        ASSERT_EQ(coords_valid,valid[i+4]);
    }
}
TEST(AVX, test_coords_valid_ltmax)
{
    //must be reverted!   c r   c r
    Point input[] = {{9,1},{1,9}}; 
    Point dim = {9,9};  //{ncol-1, nrow-1}
    auto mmDirs = _mm256_setr_epi16(-1,0,0,-1,0,1,1,0,   -1,0,0,-1,0,1,1,0);

    auto [mm_rc,mm_valid] = generateNeighbours(input,_mm_load(&dim),mmDirs);
    ALIGNED(32) int16_t rc[16];
    ALIGNED(32) int32_t valid[8];
    copyTo(mm_rc, rc);
    copyTo(mm_valid, valid);

    //print<__m256i,int16_t>("valid-16",mm_valid);
    //print<__m256i,int32_t>("valid-32",mm_valid);
    const int16_t dirs[]  = {-1,0,0,-1,0,1,1,0, -1,0,0,-1,0,1,1,0};
    const int16_t inp[]   = { 1,9,1, 9,1,9,1,9,  9,1,9, 1,9,1,9,1}; //r,c,...
    for (int i=0;i<16;++i)
    {
        auto rci = inp[i] + dirs[i];
        //printf("i %d exp %d ret %d\n", i, rci, rc[i],valid[i]);
        ASSERT_EQ(rci,rc[i]);
        if (i%2==1) {
            int32_t coords_valid = rc[i] <= 9 && rc[i-1] <= 9 ? -1 : 0;
            //printf("i %d rc[i]=%d rc[i-1]=%d valid %x\n", i, rc[i], rc[i-1],valid[i/2]);
            ASSERT_EQ(coords_valid,valid[i/2]);
        }
    }
}
/*TEST(AVX, test_process2points_manual)
{
    static ALIGNED(32) int8_t grid[] = {1,2,3,4,5,6,7,8,9};
    const int nrow=3,ncol=3;
    const int8_t color = 1;

    ALIGNED(32) int16_t _rc[] = {0,0,1,0,2,0,
                                 0,1,1,1,2,1,
                                 0,2,1,2,2,2};
    __m256i rc = _mm_load<__m256i>(_rc);
    print<__m256i,uint16_t>("rc",rc);
    auto c = _mm256_blend_epi16(rc,_mm256_setzero_si256(),0b10101010);
    auto r = _mm256_srli_epi32(_mm256_blend_epi16(rc,_mm256_setzero_si256(),0b01010101),16);
    print<__m256i,uint32_t>("r",r);
    print<__m256i,uint32_t>("c",c);
    auto idx = _mm256_add_epi32(c, _mm256_mullo_epi32(r, _mm256_set1_epi32(ncol)));
    print<__m256i,uint32_t>("idx",idx);
    auto colors = _mm256_i32gather_epi8(reinterpret_cast<const int*>(grid),idx);
    print<__m256i,uint32_t>("colors",colors);
}*/
/*TEST(AVX, test_print)
{
    auto v = _mm256_setr_epi16(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15); //bits [0:15]=0, [16:31]=1,...
    print<__m256i,int16_t>("v is",v);
    static ALIGNED(32) int8_t raw[32] = {0};
    for (int i=0;i<32;++i) raw[i]=i;
    v = _mm256_load_si256(reinterpret_cast<__m256i*>(raw));
    print<__m256i,uint16_t>("v is",v);
}*/
TEST(AVX, test_process2points)
{
    static ALIGNED(32) uint8_t grid[] = {1,1,2, 1,3,2, 1,2,2};
    static ALIGNED(32) uint8_t group_map[] = {0};
    const int nrow=3,ncol=3;
    Point input[] = {{0,0},{3,3}};  //col,row order!
    Point dim = {ncol-1,nrow-1};
    Scratchpad wrk;
    wrk.dim = _mm_load(&dim);
    wrk.dirs = _mm256_setr_epi16(-1,0,0,-1,0,1,1,0,   -1,0,0,-1,0,1,1,0);

    input[0] = {0,0};
    process2points(input, 1, grid, group_map, wrk);
    //printArr("pt_valid",outp.pt_valid,8);
    //printArr("pt_idx",outp.pt_idx,8);
    //printArr("pt_rc",outp.pt_rc,8);

    auto pts = gatherValidPoints(wrk);
    ASSERT_EQ(2, pts.size());
    ASSERT_EQ(Point(0,1), pts[0]);
    ASSERT_EQ(Point(1,0), pts[1]);

    ASSERT_EQ(1, wrk.pt_idx[2]);
    ASSERT_EQ(3, wrk.pt_idx[3]);

    /*printf("returned pts : ");
    for (int i=0;i<8;++i) 
    {
        if (wrk.pt_valid[i])
        {
            auto & p = *reinterpret_cast<Point*>(wrk.pt_rc + i);
            printf("(%d,%d,idx %d) ",p.r, p.c, wrk.pt_idx[i]);
        }
    }
    printf("\n");*/

    input[0] = {2,2};
    process2points(input, 2, grid, group_map, wrk);
    pts = gatherValidPoints(wrk);
    ASSERT_EQ(2, pts.size());
    ASSERT_EQ(Point(1,2), pts[0]);
    ASSERT_EQ(Point(2,1), pts[1]);

    ASSERT_EQ(5, wrk.pt_idx[0]);
    ASSERT_EQ(7, wrk.pt_idx[1]);

    /*printf("returned pts : ");
    for (int i=0;i<8;++i) 
    {
        if (wrk.pt_valid[i])
        {
            auto & p = *reinterpret_cast<Point*>(wrk.pt_rc + i);
            printf("(%d,%d,idx %d) ",p.r, p.c, wrk.pt_idx[i]);
        }
    }
    printf("\n");*/

    input[0] = {1,1};
    process2points(input, 3, grid, group_map, wrk);
    pts = gatherValidPoints(wrk);
    ASSERT_EQ(0, pts.size());
}
TEST(AVX, test_fillGroupFromPosition)
{
    uint8_t grid[] = {1,1,2, 1,3,2, 1,2,2};

    const int nrow=3,ncol=3;
    const int totSize = nrow*ncol;
    Point seed = {0,0};
    Point dim = {ncol-1,nrow-1};

    Workspace wrk(nrow,ncol);
    auto pss = SearchState::alloc(nrow,ncol);
    memcpy(pss->grid,grid,sizeof(grid[0])*totSize);
    wrk.clearGroupMap(totSize);

    fillGroupFromPositionAVX(*pss, dim, seed, 1,1, wrk);

    const uint8_t exp_groups[] = {1,1,0,1,0,0,1,0,0};
    for (int i=0;i<totSize;++i)
        ASSERT_EQ(exp_groups[i], wrk.group_map[i]);

    delete pss;
}
TEST(AVX, test_fillGroupFromPosition_2)
{
    const int nrow=5,ncol=5;
    const int totSize = nrow*ncol;
    Point seed = {0,0};
    Point dim = {ncol-1,nrow-1};

    Workspace wrk(nrow,ncol);
    auto * pss= makeGrid({  "11122",
                            "31344",
                            "33354",
                            "67855",
                            "68795"});

    wrk.clearGroupMap(totSize);

    fillGroupFromPositionAVX(*pss, dim, seed, 1,1, wrk);

    const uint8_t exp_groups[] = {1,1,1,0,0, 0,1,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
    for (int i=0;i<totSize;++i)
        ASSERT_EQ(exp_groups[i], wrk.group_map[i]);

    delete pss;
}