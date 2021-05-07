//
// Created by barabasz on 05.05.2021.
//

#include <gtest/gtest.h>
#include <board.h>

using namespace Pac;
using namespace std;

Board makeCrossBoard()
{
    Board board;
    board.fields = {1,2,3,4,5};
    board.graph  = {{1,{3}}, {2,{3}}, {3,{1,2,4,5}}, {4,{3}}, {5,{3}} };
    return board;
}
Board makeMaze1()
{
     vector<string> maze = {"%--------------%---%",
                            "%-%%-%%-%%-%%-%%-%-%",
                            "%----------------%-%",
                            "%%%%%%%%%%%%%%%%%%-%",
                            "%------------------%"};
     return Board::create(maze, '-', '%');
}
Board makeMaze2()
{
    vector<string> maze = {"%%%%%%%%%%%%%%%%%%%%%%%%%%%%",
                           "%------------%%------------%",
                           "%-%%%%-%%%%%-%%-%%%%%-%%%%-%",
                           "%-%%%%-%%%%%-%%-%%%%%-%%%%-%",
                           "%-%%%%-%%%%%-%%-%%%%%-%%%%-%",
                           "%--------------------------%",
                           "%-%%%%-%%-%%%%%%%%-%%-%%%%-%",
                           "%-%%%%-%%-%%%%%%%%-%%-%%%%-%",
                           "%------%%----%%----%%------%",
                           "%%%%%%-%%%%%-%%-%%%%%-%%%%%%",
                           "%%%%%%-%%%%%-%%-%%%%%-%%%%%%",
                           "%%%%%%-%------------%-%%%%%%",
                           "%%%%%%-%-%%%%--%%%%-%-%%%%%%",
                           "%--------%--------%--------%",
                           "%%%%%%-%-%%%%%%%%%%-%-%%%%%%",
                           "%%%%%%-%------------%-%%%%%%",
                           "%%%%%%-%-%%%%%%%%%%-%-%%%%%%",
                           "%------------%%------------%",
                           "%-%%%%-%%%%%-%%-%%%%%-%%%%-%",
                           "%-%%%%-%%%%%-%%-%%%%%-%%%%-%",
                           "%---%%----------------%%---%",
                           "%%%-%%-%%-%%%%%%%%-%%-%%-%%%",
                           "%%%-%%-%%-%%%%%%%%-%%-%%-%%%",
                           "%------%%----%%----%%------%",
                           "%-%%%%%%%%%%-%%-%%%%%%%%%%-%",
                           "%--------------------------%",
                           "%%%%%%%%%%%%%%%%%%%%%%%%%%%%"};
    return Board::create(maze, '-', '%');
}

TEST(PacBoard, makeCrossBoard)
{
    Board board = makeCrossBoard();
    auto & ne1 = board.graph[1];
    ASSERT_EQ(1, ne1.cnt);
    ASSERT_EQ(3, ne1.ids[0]);

    auto & ne2 = board.graph[3];
    ASSERT_EQ(4, ne2.cnt);
    ASSERT_EQ(1, ne2.ids[0]);
    ASSERT_EQ(2, ne2.ids[1]);
    ASSERT_EQ(4, ne2.ids[2]);
    ASSERT_EQ(5, ne2.ids[3]);
}
TEST(PacBoard, findPath_crossboard_12)
{
    Board board = makeCrossBoard();
    auto path = board.findPath(1,2);
    ASSERT_EQ(3,path.size());
    ASSERT_EQ(1,path[0]);
    ASSERT_EQ(3,path[1]);
    ASSERT_EQ(2,path[2]);
}
TEST(PacBoard, findPath_crossboard_13)
{
    Board board = makeCrossBoard();
    auto path = board.findPath(1,3);
    ASSERT_EQ(2,path.size());
    ASSERT_EQ(1,path[0]);
    ASSERT_EQ(3,path[1]);
}
TEST(PacBoard, findPath_crossboard_15)
{
    Board board = makeCrossBoard();
    auto path = board.findPath(1,5);
    ASSERT_EQ(3,path.size());
    ASSERT_EQ(1,path[0]);
    ASSERT_EQ(3,path[1]);
    ASSERT_EQ(5,path[2]);
}
TEST(PacBoard, findPath_crossboard_54)
{
    Board board = makeCrossBoard();
    auto path = board.findPath(5,4);
    ASSERT_EQ(3,path.size());
    ASSERT_EQ(5,path[0]);
    ASSERT_EQ(3,path[1]);
    ASSERT_EQ(4,path[2]);
}
TEST(PacBoard, findPath_maze1_01_41)
{
    Board board = makeMaze1();
    auto from = Board::pos2id(0, 1);
    auto to   = Board::pos2id(4, 1);
    auto path = board.findPath(from, to);
    ASSERT_EQ(43,path.size());
    ASSERT_EQ(to, path.back());
}
TEST(PacBoard, findPath_maze1_014_41)
{
    Board board = makeMaze1();
    auto from = Board::pos2id(0, 14);
    auto to   = Board::pos2id(4, 1);
    auto path = board.findPath(from, to);
    ASSERT_EQ(32,path.size());
    ASSERT_EQ(to, path.back());
}
TEST(PacBoard, findPath_maze1_029_51)
{
    Board board = makeMaze1();
    auto from = Board::pos2id(2, 9);
    auto to   = Board::pos2id(4, 1);
    auto path = board.findPath(from, to);
    ASSERT_EQ(33,path.size());
    ASSERT_EQ(to, path.back());
}
TEST(PacBoard, findPath_maze2_2513_0301)
{
    Board board = makeMaze2();
    auto from = Board::pos2id(25, 13);
    auto to   = Board::pos2id(3, 1);
    auto path = board.findPath(from, to);
    ASSERT_EQ(35,path.size());
    ASSERT_EQ(to, path.back());
}