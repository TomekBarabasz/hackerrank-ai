#include <gtest/gtest.h>
#include <fstream>
#include "click-o-mania.h"

using namespace Click_o_mania;
using Pt = BlockList<int8_t>::Point;

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
    auto * pss = makeGrid({"112","132","122"});
    auto & ss = *pss;
    Workspace wrk(ss.nrow,ss.ncol);
    //auto wrk = Workspace::make(ss.nrow,ss.ncol);
    // @test partition(grid) == [1 1 2;1 3 2;1 2 2]
    int ng = partitionGrid(ss,wrk);
    ASSERT_EQ(3,ng);
    const int8_t exp[] = {1, 1, 2, 1, 3, 2, 1, 2, 2};
    for (int idx=0;idx<ss.nrow*ss.ncol;++idx){
        ASSERT_EQ(exp[idx], wrk.group_map[idx]);
    }
    delete pss;
}
TEST(Partition,test_011)
{
    auto * pss = makeGrid({"112","132","122"});
    auto & ss = *pss;
    auto rwrk = Ref(Workspace::make(ss.nrow,ss.ncol));
    auto & wrk = rwrk.get();
    // @test partition(grid) == [1 1 2;1 3 2;1 2 2]
    int ng = partitionGrid(ss,wrk);
    ASSERT_EQ(3,ng);
    const int8_t exp[] = {1, 1, 2, 1, 3, 2, 1, 2, 2};
    for (int idx=0;idx<ss.nrow*ss.ncol;++idx){
        ASSERT_EQ(exp[idx], wrk.group_map[idx]);
    }
    delete pss;
}
TEST(Partition,test_02)
{
    auto * pss= makeGrid({"11122",
                    "31344",
                    "33354",
                    "67855",
                    "68795"});
    auto & ss = *pss;
    Workspace wrk(ss.nrow,ss.ncol);
    //@test partition(grid) == [1 1 1 2 2;3 1 3 4 4;3 3 3 5 4;6 7 8 5 5;6 9 10 11 5]
    int ng = partitionGrid(ss,wrk);
    ASSERT_EQ(11,ng);
    const int8_t exp[] = {1, 1, 1, 2, 2, 3, 1, 3, 4, 4, 3, 3, 3, 5, 4, 6, 7, 8, 5, 5, 6, 9, 10, 11, 5};
    for (int idx=0;idx<ss.nrow*ss.ncol;++idx){
        ASSERT_EQ(exp[idx], wrk.group_map[idx]);
    }
    delete pss;
}
TEST(CollectGroups,test_01)
{
    auto *pss = makeGrid({"112","132","122"});
    auto & ss = *pss;
    Workspace wrk(ss.nrow,ss.ncol);
    // @test partition(grid) == [1 1 2;1 3 2;1 2 2]
    int ng = partitionGrid(ss,wrk);
    ASSERT_EQ(3,ng);
    collectGroups(ss.nrow,ss.ncol,ng,wrk,ss.blocks);
    auto & blocks = ss.blocks;
    ASSERT_EQ(3,blocks.ngroups);
    //group 1
    {
        auto [id,first,nelem] = blocks.groups[0];
        ASSERT_EQ(1,id);
        ASSERT_EQ(0,first);
        ASSERT_EQ(4,nelem);
        ASSERT_EQ( Pt(0,0),blocks.points[0]);
        ASSERT_EQ( Pt(0,1),blocks.points[1]);
        ASSERT_EQ( Pt(1,0),blocks.points[2]);
        ASSERT_EQ( Pt(2,0),blocks.points[3]);
    }
    //group 2
    {
        auto [id,first,nelem] = blocks.groups[1];
        ASSERT_EQ(2,id);
        ASSERT_EQ(4,first);
        ASSERT_EQ(4,nelem);
        ASSERT_EQ( Pt(0,2),blocks.points[4]);
        ASSERT_EQ( Pt(1,2),blocks.points[5]);
        ASSERT_EQ( Pt(2,1),blocks.points[6]);
        ASSERT_EQ( Pt(2,2),blocks.points[7]);
    }
    //group 3
    {
        auto [id,first,nelem] = blocks.groups[2];
        ASSERT_EQ(3,id);
        ASSERT_EQ(8,first);
        ASSERT_EQ(1,nelem);
        ASSERT_EQ( Pt(1,1),blocks.points[8]);
    }
    delete pss;
}
TEST(updateColumn,test_01)
{
    auto *pss = makeGrid({"123",
                          "345",
                          "687"});
    const bool empty = updateColumn(*pss,0);
    ASSERT_FALSE(empty);
    int8_t exp[] = {'1','2','3','3','4','5','6','8','7'};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(updateColumn,test_02)
{
    auto *pss = makeGrid({" 23",
                          "313",
                          "687"});
    const bool empty = updateColumn(*pss,0);
    ASSERT_FALSE(empty);
    int8_t exp[] = {' ','2','3','3','1','3','6','8','7'};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(updateColumn,test_03)
{
    auto *pss = makeGrid({"123",
                          " 13",
                          "687"});
    const bool empty = updateColumn(*pss,0);
    ASSERT_FALSE(empty);
    int8_t exp[] = {' ','2','3','1','1','3','6','8','7'};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(updateColumn,test_04)
{
    auto *pss = makeGrid({"123",
                          "313",
                          " 87"});
    const bool empty = updateColumn(*pss,0);
    ASSERT_FALSE(empty);
    //@test updateColumn(grid,1) == makeGrid([" 23","113","387"])
    int8_t exp[] = {' ','2','3','1','1','3','3','8','7'};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(updateColumn,test_05)
{
    auto *pss = makeGrid({" 23",
                          " 13",
                          " 87"});
    const bool empty = updateColumn(*pss,0);
    ASSERT_TRUE(empty);
    delete pss;
}
TEST(updateColumn,test_06)
{
    auto *pss = makeGrid({"123",
                          "3 3",
                          "4 7"});
    const bool empty = updateColumn(*pss,1);
    ASSERT_FALSE(empty);
    int8_t exp[] = {'1',' ','3','3',' ','3','4','2','7'};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(removeColumn,test_01)
{
    auto *pss = makeGrid({" 23",
                          " 13",
                          " 87"});
    removeColumn(*pss,0);
    ASSERT_EQ(2,pss->ncol);
    int8_t exp[] = {'2','3','1','3','8','7'};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(removeColumn,test_02)
{
    auto *pss = makeGrid({"2 3",
                          "1 3",
                          "8 7"});
    removeColumn(*pss,1);
    ASSERT_EQ(2,pss->ncol);
    int8_t exp[] = {'2','3','1','3','8','7'};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(removeColumn,test_03)
{
    auto *pss = makeGrid({"23 ",
                          "13 ",
                          "87 "});
    removeColumn(*pss,2);
    ASSERT_EQ(2,pss->ncol);
    int8_t exp[] = {'2','3','1','3','8','7'};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(removeEmptyRows,test_01)
{
    auto *pss = makeGrid({" 23",
                          "4 6",
                          "78 "});
    removeEmptyRows(*pss);
    ASSERT_EQ(3,pss->nrow);
    int8_t exp[] = {' ','2','3','4',' ','6','7','8',' '};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(removeEmptyRows,test_02)
{
    auto *pss = makeGrid({"   ",
                          "4 6",
                          "78 "});
    removeEmptyRows(*pss);
    ASSERT_EQ(2,pss->nrow);
    int8_t exp[] = {'4',' ','6','7','8',' '};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(removeEmptyRows,test_03)
{
    auto *pss = makeGrid({"   ",
                          "   ",
                          "78 "});
    removeEmptyRows(*pss);
    ASSERT_EQ(1,pss->nrow);
    int8_t exp[] = {'7','8',' '};
    for (int idx=0;idx<pss->nrow*pss->ncol;++idx)
        ASSERT_EQ(exp[idx], pss->grid[idx]);
    delete pss;
}
TEST(removeEmptyRows,test_04)
{
    auto *pss = makeGrid({"   ",
                          "   ",
                          "   "});
    removeEmptyRows(*pss);
    ASSERT_EQ(0,pss->nrow);
    delete pss;
}
TEST(removeGroup,test_01)
{
    auto *pss = makeGrid({"brbb",
                          "rryy",
                          "yrby",
                          "brry",
                          "yrrb"});
    Workspace wrk(pss->nrow,pss->ncol);
    int ng = partitionGrid(*pss,wrk);
    ASSERT_EQ(9,ng);
    collectGroups(pss->nrow,pss->ncol,ng,wrk,pss->blocks);
    auto *nss = removeGroup(*pss,1,wrk);
    auto * grid = nss->grid;
    /*for(int r=0;r<nss->nrow;++r)
    {
        for (int c=0;c<nss->ncol;++c) printf("%c",*grid++);
        printf("\n");
    }*/
    ASSERT_EQ(3,nss->ncol);
    ASSERT_EQ(5,nss->nrow);
    int8_t exp[] = {' ',' ','b', 'b',' ','y', 'y','b','y', 'b','y','y', 'y','b','b'};
    for (int idx=0;idx<nss->nrow*nss->ncol;++idx)
        ASSERT_EQ(exp[idx], nss->grid[idx]);
    delete pss;
    delete nss;
}
TEST(solve,test_01)
{
    const auto res = solve({"y  ",
                            "y  ",
                            "r  "});
    ASSERT_EQ(Pt(0,0),res);
}
TEST(solve,test_02)
{
    const auto res = solve({"b  ",
                            "y  ",
                            "bbb",
                            "ybb"});
    ASSERT_EQ(Pt(2,0),res);
}
TEST(solve,test_03)
{
    const auto res = solve({"  b",
                            "b y",
                            "yby",
                            "byy",
                            "ybb"});
    ASSERT_EQ(Pt(4,1),res);
}
TEST(solve,test_04)
{
    const auto res = solve({"brbb",
                            "rryy",
                            "yrby",
                            "brry",
                            "yrrb"});
    //ASSERT_EQ(Pt(4,1),res);
}
