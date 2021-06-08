//
// Created by barabasz on 20.05.2021.
//
#include "maze-escape.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

using namespace MazeEscape;
using namespace std;

Area makeMaze(int mazeId)
{
    switch(mazeId)
    {
        default:
        case 1:{
            vector<string> maze = {"#######",
                                   "#--#--#",
                                   "#--#--#",
                                   "#--#--#",
                                   "#-----#",
                                   "###e###"};
            return maze;}
        case 2:{
            vector<string> maze = {"#######",
                                   "#--#--#",
                                   "#--#--#",
                                   "#--#-##",
                                   "#-----#",
                                   "###e###"};
            return maze;}
        case 3:{
            vector<string> maze = {"#######",
                                   "#--#--#",
                                   "#--#--#",
                                   "#--#-##",
                                   "#-----e",
                                   "#######"};
            return maze;}
        case 4: {
            vector<string> maze = {"############################",
                                   "#------------##------------#",
                                   "#--#-----##-----####---##--#",
                                   "#--#-----##-----####---##--#",
                                   "#--###---##-----####---##--#",
                                   "#--------------------------#",
                                   "#--------------------------#",
                                   "#--#-----##-----####---##--#",
                                   "#------##----##----##------#",
                                   "#---##----------------##---#",
                                   "#------##----##----##------#",
                                   "#--###----##----##---------#",
                                   "#--------------------------#",
                                   "##########################e#"};
            return maze;}
        case 5: {
            vector<string> maze = {"############################",
                                   "#------------##------------#",
                                   "#--#-----##-----####---##--#",
                                   "#--#-----##-----####---##--#",
                                   "#--###---##-----####---##--#",
                                   "#--------------------------#",
                                   "#--------------------------#",
                                   "#--#-----##-----####---##--#",
                                   "#------##----##----##------#",
                                   "#---##----------------##---#",
                                   "#------##----##----##------#",
                                   "#--###----##----##---------#",
                                   "#--------------------------#",
                                   "#e##########################"};
            return maze;}
    }
}

string dirToString(int dir)
{
    static const map<int, string> Dirs =
    {
            {0, "UP"},
            {1, "RIGHT"},
            {2, "DOWN"},
            {3, "LEFT"}
    };
    return Dirs.at(dir);
}

int main(int argc, char** argv)
{
    static const char *tmpFilename = "tmpfile.txt";
    if (1==argc) {
        cout << "usage: maze-escape mazeid _posr _posc _dir nmoves" << endl;
        return 1;
    }
    const int mazeId = atoi(argv[1]);
    const auto maze = makeMaze(mazeId);
    const int ROWS = maze.rows();
    const int COLS = maze.cols();

    int posr = argc > 2 ? atoi(argv[2]) : -1;
    int posc = argc > 3 ? atoi(argv[3]) : -1;
    int dir = argc > 4 ? atoi(argv[4]) : -1;
    int nmoves = argc > 5 ? atoi(argv[5]) : 1000;
    if (posr < 0) posr = rand() % ROWS;
    if (posc < 0) posc = rand() % COLS;
    if (dir < 0) dir = rand() % 4;
    cout << "_posr " << posr << " _posc " << posc << " _dir " << dir << endl;

    std::ofstream tmp(tmpFilename);
    vector<int> moves;
    Agent *agent = Agent::create("nomem_1");

    while( maze[posr][posc] != 'e' && nmoves-->0 )
    {
        auto view = maze.getView(posr,posc,dir);
        const int move = agent->makeMove(view);
        moves.push_back(move);
        dir = (dir+move) % 4;
        Agent::updatePos(posr,posc,dir);
        if (posr<0 || posc<0 || posr>=ROWS || posc>=COLS || maze.isWall(posr,posc)){
            cout << "agent specified invalid move, next postion is (" << posr << "," << posc << ")" << endl;
            return 1;
        }
        //for (auto & r : view) cout << r << std::endl;
        //cout << dirToString(move) << " new _dir " << dirToString(_dir) << " new pos " << _posr << " " << _posc << std::endl;
    }
    if (maze[posr][posc] != 'e') {
        cout << "failed" << endl;
    }else{
        cout << "found exit within " << moves.size() << "moves" << endl;
    }

    auto ea = agent->getExploredArea();
    cout << "explored area " << endl;
    for (int i=0; i < ea.rows(); ++i)
        cout << ea.getRow(i) << endl;
    
    //for (auto & m : moves){
    //    cout<< dirToString(m) << std::endl;
    //}

    std::remove(tmpFilename);
    delete agent;
    return 0;
}
