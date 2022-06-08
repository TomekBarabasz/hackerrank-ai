#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <immintrin.h>
#include <gtest/gtest.h>

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

namespace Click_o_mania
{
//NOTE: coords order is columns,rows (to be loaded by _epi32 -> yielding r,c in avx registers)
using Point16_t = std::tuple<int16_t,int16_t>;

inline __m256i _mm256_i32gather_epi8 (int const* base_addr, __m256i vindex)
{
    __m256i v32index = _mm256_srli_epi32(vindex,2);
    __m256i shift = _mm256_slli_epi32(_mm256_and_si256(vindex, _mm256_set1_epi32(3)),3);
    __m256i values = _mm256_i32gather_epi32(base_addr, v32index, 4);
    values = _mm256_srlv_epi32(values, shift);
    return  _mm256_and_si256(values,_mm256_set1_epi32(0xff));
}
//NOTE: dim is {ncol-1,nrow-1}

inline std::tuple<__m256i,__m256i> checkIfNeighboursValid(Point16_t& dim_,Point16_t inp[2])
{
    static ALIGNED(32) int16_t idirs[] = {-1,0,0,-1,0,1,1,0,   -1,0,0,-1,0,1,1,0};
    __m256i dirs = _mm256_load_si256(reinterpret_cast<__m256i const*>(idirs));
    __m256i rc = _mm256_permute2f128_si256(_mm256_set1_epi32(*reinterpret_cast<int*>(inp)), _mm256_set1_epi32(*reinterpret_cast<int*>(inp+1)),0x21);
    //print<__m256i,int16_t>("rc",rc);
    //print<__m256i,int16_t>("dirs",dirs);
    rc = _mm256_add_epi16(rc,dirs);
    //print<__m256i,int16_t>("newrc",rc);
    __m256i dim = _mm256_set1_epi32(*reinterpret_cast<const int*>(&dim_));
    //print<__m256i,int16_t>("dim",dim);
    //a>b?0xFFFF:0
    //0xFFFF if coord is invalid
    auto valid = _mm256_or_si256(_mm256_cmpgt_epi16(_mm256_setzero_si256(),rc), _mm256_cmpgt_epi16(rc,dim));
    valid = _mm256_cmpeq_epi32(valid, _mm256_setzero_si256());
    return {rc,valid};
}

struct WrkOutp
{
    int32_t pt_idx[8], pt_valid[8], pt_rc[8];
};
//NOTE: dim is {ncol-1,nrow-1}
inline void process2points(Point16_t& dim,Point16_t inp[2], int8_t color, uint8_t group, const int8_t * grid, uint8_t * group_map, WrkOutp& outp)
{
    ALIGNED(32) int32_t pt_idx[8], pt_valid[8], pt_rc[8];
    auto [rc,valid] = checkIfNeighboursValid(dim,inp);
    auto r = _mm256_blend_epi16(rc,_mm256_setzero_si256(),0b01010101);
    auto c = _mm256_blend_epi16(rc,_mm256_setzero_si256(),0b10101010);
    auto idx = _mm256_add_epi32(c, _mm256_mullo_epi32(r, _mm256_set1_epi32(get<1>(dim)+1)));
    idx = _mm256_and_si256(idx,valid);
    auto colors = _mm256_i32gather_epi8(reinterpret_cast<const int*>(grid),idx);
    auto groups = _mm256_i32gather_epi8(reinterpret_cast<const int*>(group_map),idx);
    valid = _mm256_and_si256(valid, _mm256_cmpeq_epi32(colors,_mm256_set1_epi32(color)));
    valid = _mm256_and_si256(valid, _mm256_cmpeq_epi32(groups,_mm256_setzero_si256()));
    rc =  _mm256_or_si256(rc,valid);

    _mm256_store_si256((__m256i*)outp.pt_idx,idx);
    _mm256_store_si256((__m256i*)outp.pt_valid,valid);
    _mm256_store_si256((__m256i*)outp.pt_rc,rc);
}



inline void fillGroupFromPositionAVX(int nrow,int ncol, int r0, int c0, uint8_t color,uint8_t group, const int8_t * grid, uint8_t * group_map)
{
    vector<Point16_t> queue;
    Point16_t dim = {nrow-1,ncol-1};
    Point16_t inp[2];
    ALIGNED(32) WrkOutp outp;

    inp[0] = {r0,c0};
    queue.push_back(inp[0]);
    while (!queue.empty())
    {
        inp[0] = queue.back();
        queue.pop_back();
        if (!queue.empty()) {
            inp[1] = queue.back();
            queue.pop_back();
        } else {
            inp[1] = {nrow,ncol};
        }
        process2points(dim,inp, color, group, grid, group_map, outp);
        for (int i=0;i<8;++i)
            if (outp.pt_valid[i])
            {
                group_map[outp.pt_idx[i]] = group;
                queue.push_back( *reinterpret_cast<Point16_t*>(outp.pt_rc + i) );
            }
    }
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
    Point16_t input[] = {{0,0},{1,1}};
    Point16_t dim = {9,9};

    auto [mm_rc, mm_valid] = checkIfNeighboursValid(dim,input);
    ALIGNED(32) int16_t rc[16];
    ALIGNED(32) int32_t valid[8];
    copyTo(mm_rc, rc);
    copyTo(mm_valid, valid);

    //print<__m256i,int16_t>("valid-16",mm_valid);
    //print<__m256i,int32_t>("valid-32",mm_valid);
    const int16_t dirs[]  = {-1,0,0,-1,0,1,1,0, -1,0,0,-1,0,1,1,0};
    const int16_t inp[]   = { 0,0,0, 0,0,0,0,0,  1,1,1, 1,1,1,1,1};
    for (int i=0;i<16;++i)
    {
        auto rci = inp[i] + dirs[i];
        //printf("i %d exp %d ret %d\n", i, rci, rc[i]);
        ASSERT_EQ(rci,rc[i]);
        if (i%2==1) {
            int32_t coords_valid = rc[i] >=0 && rc[i-1] >=0 ? -1 : 0;
            //printf("i %d rc[i]=%d rc[i-1]=%d valid %x\n", i, rc[i], rc[i-1],valid[i/2]);
            ASSERT_EQ(coords_valid,valid[i/2]);
        }
    }
}
TEST(AVX, test_coords_valid_ltmax)
{
    //must be reverted!   c r   c r
    Point16_t input[] = {{9,1},{1,9}}; 
    Point16_t dim = {9,9};  //{ncol-1, nrow-1}

    auto [mm_rc,mm_valid] = checkIfNeighboursValid(dim,input);
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

/*TEST(AVX, test_process2points)
{
    static ALIGNED(32) int8_t grid[] = {0,1,2,3,4,5,6,7,8};
    static ALIGNED(32) uint8_t group_map[] = {0,1,2,3,4,5,6,7,8};
    const int nrow=3,ncol=3;
    Point16_t input[] = {{2,3},{4,5}};
    const int8_t color = 1;
    const uint8_t group = 1;
    Point16_t outp[8] = {};
    Point16_t dim = {8,9};

    process2points(dim, input, color, group, grid, group_map, outp);
}*/