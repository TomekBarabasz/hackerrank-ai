//
// Created by barabasz on 07.06.2021.
//
#include "maze-escape.h"

namespace MazeEscape
{
    int Agent::makeMove(const Area &view, ofstream &storage)
    {
        const int exitp = view.find('e');
        return exitp < 0 ? goAlongAwall(view) : moveToPos(view, exitp);
    }
    int Agent::goAlongAwall(const Area &area)
    {
        string a = area[0] + area[1] + area[2];
        const string wall_front = "###------";
        int dir;
        dir = area.matchFeature(wall_front);
        if (dir >= 0) {
            return (dir + 1) % 4;
        }

        const string corner_left = "####--#--";
        dir = area.matchFeature(corner_left);
        if (dir >= 0) {
            return (dir + 1) % 4;
        }

        if (area[0][1] == '-') return 0;
        if (area[0][0] == '-') return 3;
        if (area[0][2] == '-') return 1;

        return rand() % 4;
    }
    int Agent::moveToPos(const Area &view, int pos)
    {
        switch (pos) {
            case 0:
                if (view[0][1] == '-') return 0;
                if (view[1][0] == '-') return 3;
                return oneOf(1, 2);
            case 1:
                return 0;
            case 2:
                if (view[0][1] == '-') return 0;
                if (view[1][2] == '-') return 1;
                return oneOf(2, 3);
            case 3:
                return 3;
            case 5:
                return 1;
            case 6:
                if (view[1][0] == '-') return 3;
                if (view[2][1] == '-') return 2;
                return oneOf(0, 1);
            case 7:
                return 2;
            case 8:
                if (view[1][2] == '-') return 1;
                if (view[2][1] == '-') return 2;
                return oneOf(3, 0);
            default:
                return rand() % 4;
        }
    }
    int Agent::makeRandomMove(const Area &view)
    {
        vector<int> dirs;
        if (view[0][1] == '-') dirs.push_back(0);
        if (view[1][0] == '-') dirs.push_back(1);
        if (view[2][1] == '-') dirs.push_back(2);
        if (view[1][2] == '-') dirs.push_back(3);
        return dirs[rand() % dirs.size()];
    }
}