//
// Created by barabasz on 06.05.2021.
//
#include <gtest/gtest.h>
#include <pacman.h>

using namespace Pac;
using namespace std;

TEST(Pac, getMove_LEFT)
{
    auto from = Board::pos2id(1,1);
    auto to = Board::pos2id(0,1);
    ASSERT_EQ("LEFT",Pac::getMove(from,to));
}

TEST(Pac, getMove_RIGHT)
{
    auto from = Board::pos2id(1,1);
    auto to = Board::pos2id(2,1);
    ASSERT_EQ("RIGHT",Pac::getMove(from,to));
}

TEST(Pac, getMove_UP)
{
    auto from = Board::pos2id(1,1);
    auto to = Board::pos2id(1,0);
    ASSERT_EQ("UP",Pac::getMove(from,to));
}

TEST(Pac, getMove_DOWN)
{
    auto from = Board::pos2id(1,1);
    auto to = Board::pos2id(1,2);
    ASSERT_EQ("DOWN",Pac::getMove(from,to));
}