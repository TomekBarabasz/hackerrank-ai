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

void updatePos(int& posr,int& posc, int dir)
{
    switch(dir){
        case 0: posr--; break;
        case 1: posc++; break;
        case 2: posr++; break;
        case 3: posc--; break;
    }
}

int main(int argc, char** argv)
{
    static const char *tmpFilename = "tmpfile.txt";
    if (1==argc) {
        cout << "usage: maze-escape mazeid posr posc dir nmoves" << endl;
        return 1;
    }
    const int mazeId = atoi(argv[1]);
    int posr = argc > 2 ? atoi(argv[2]) : -1;
    int posc = argc > 3 ? atoi(argv[3]) : -1;
    int dir = argc > 4 ? atoi(argv[4]) : -1;
    int nmoves = argc > 5 ? atoi(argv[5]) : 1000;

    const auto maze = makeMaze(mazeId);
    const int ROWS = maze.rows();
    const int COLS = maze.cols();

    if (posr < 0) posr = rand() % ROWS;
    if (posc < 0) posc = rand() % COLS;

    if (dir < 0) dir = rand() % 4;

    cout << "posr " << posr << " posc " << posc << " dir " << dir << endl;

    std::ofstream tmp(tmpFilename);
    vector<int> moves;
    Area exploredArea;
    int rel_posr=0, rel_posc=0, rel_dir=0;
    Agent agent;
    while( maze[posr][posc] != 'e' && nmoves-->0 )
    {
        auto view = maze.getView(posr,posc,dir);
        const int move = agent.makeMove(view, tmp);
        Area aligned = view.alingNorth(dir);
        exploredArea.append(aligned, rel_posr, rel_posc, dir, '.');
        moves.push_back(move);
        dir = (dir+move) % 4;
        updatePos(posr,posc,dir);
        updatePos(rel_posr,rel_posc,dir);
        //for (auto & r : view) cout << r << std::endl;
        //cout << dirToString(move) << " new dir " << dirToString(dir) << " new pos " << posr << " " << posc << std::endl;
    }
    if (maze[posr][posc] != 'e') {
        cout << "failed" << endl;
    }else{
        cout << "found exit within " << moves.size() << "moves" << endl;
    }
    
    cout << "explored area_ " << std::endl;
    for (int i=0; i < exploredArea.rows(); ++i)
        cout << exploredArea.getRow(i) << endl;
    
    //for (auto & m : moves){
    //    cout<< dirToString(m) << std::endl;
    //}

    std::remove(tmpFilename);

    return 0;
}
