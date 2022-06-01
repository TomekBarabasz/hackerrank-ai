#include <gtest/gtest.h>
#include <fstream>
#include "click-o-mania.h"

using namespace Click_o_mania;

TEST(TestRing, test_01)
{
    Ring<int> ring(3);

    ASSERT_TRUE(ring.empty());
    ASSERT_TRUE(ring.push(1));
    ASSERT_FALSE(ring.empty());
    ASSERT_TRUE(ring.push(2));
    ASSERT_TRUE(ring.push(3));
    ASSERT_FALSE(ring.push(4));
    ASSERT_EQ(1,ring.popfirst());
    ASSERT_EQ(2,ring.popfirst());
    ASSERT_EQ(3,ring.popfirst());
}
TEST(TestRing, test_02)
{
    Ring<int> ring(3);

    ring.push(1);
    ring.push(2);
    ring.push(3);
    ASSERT_EQ(1,ring.popfirst());
    ASSERT_TRUE(ring.push(4));
    ASSERT_EQ(2,ring.popfirst());
    ASSERT_TRUE(ring.push(5));
    ASSERT_EQ(3,ring.popfirst());
    ASSERT_TRUE(ring.push(6));
    ASSERT_EQ(4,ring.popfirst());
    ASSERT_EQ(5,ring.popfirst());
    ASSERT_EQ(6,ring.popfirst());
}

TEST(Partition,test_01)
{
    auto ss = makeGrid({"112","132","122"});
    Workspace wrk(ss.nrow,ss.ncol);
    //auto wrk = Workspace::make(ss.nrow,ss.ncol);
    // @test partition(grid) == [1 1 2;1 3 2;1 2 2]
    int ng = partitionGrid(ss,wrk);
    ASSERT_EQ(3,ng);
    const int8_t exp[] = {1, 1, 2, 1, 3, 2, 1, 2, 2};
    for (int idx=0;idx<ss.nrow*ss.ncol;++idx){
        ASSERT_EQ(exp[idx], wrk.group_map[idx]);
    }
}
TEST(Partition,test_011)
{
    auto ss = makeGrid({"112","132","122"});
    auto rwrk = Ref(Workspace::make(ss.nrow,ss.ncol));
    auto & wrk = rwrk.get();
    // @test partition(grid) == [1 1 2;1 3 2;1 2 2]
    int ng = partitionGrid(ss,wrk);
    ASSERT_EQ(3,ng);
    const int8_t exp[] = {1, 1, 2, 1, 3, 2, 1, 2, 2};
    for (int idx=0;idx<ss.nrow*ss.ncol;++idx){
        ASSERT_EQ(exp[idx], wrk.group_map[idx]);
    }
}
TEST(Partition,test_02)
{
    auto ss= makeGrid({"11122",
                    "31344",
                    "33354",
                    "67855",
                    "68795"});
    Workspace wrk(ss.nrow,ss.ncol);
    //@test partition(grid) == [1 1 1 2 2;3 1 3 4 4;3 3 3 5 4;6 7 8 5 5;6 9 10 11 5]
    int ng = partitionGrid(ss,wrk);
    ASSERT_EQ(11,ng);
    const int8_t exp[] = {1, 1, 1, 2, 2, 3, 1, 3, 4, 4, 3, 3, 3, 5, 4, 6, 7, 8, 5, 5, 6, 9, 10, 11, 5};
    for (int idx=0;idx<ss.nrow*ss.ncol;++idx){
        ASSERT_EQ(exp[idx], wrk.group_map[idx]);
    }
}