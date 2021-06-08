//
// Created by barabasz on 07.06.2021.
//
#include <cstring>
#include "maze-escape.h"

namespace MazeEscape
{
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
    void Agent::updatePos(int& posr,int& posc, int dir)
    {
        switch(dir){
            case 0: posr--; break;
            case 1: posc++; break;
            case 2: posr++; break;
            case 3: posc--; break;
        }
    }

    struct MemorylessAgent1 : Agent
    {
        int rot90cw(int dir) { return (dir+1) % 4; }
        int makeMove(const Area &area) override
        {
            string a = area[0] + area[1] + area[2];
            string features[] = {"###------", "#-#------","####--#--" };
            for (const auto & f : features) {
                int dir = area.matchFeature(f);
                if (dir >= 0) return rot90cw(dir);
            }

            if (area[0][1] == '-') return 0;
            if (area[0][0] == '-') return 3;
            if (area[0][2] == '-') return 1;

            return rand() % 4;
        }
    };

    struct MemorylessAgent2 : Agent
    {
        int makeMove(const Area &area) override
        {
            if (area[0]=="#-#") return RIGHT;
            if (!area.isWall(0,0) && !area.isWall(1,0) && area.isWall(2,0)) return LEFT;
            if (!area.isWall(0,1)) return UP;

            return rand() % 4;
        }
    };

    struct MemoryAgent : Agent
    {
        int makeMoveImpl(const Area &view)
        {
            return rand() % 4;
        }

        int makeMove(const Area &view) override
        {
            Area aligned = view.alingNorth(_dir);
            exploredArea.append(aligned, _posr, _posc, _dir, '.');

            const int exitp = view.find('e');
            auto move = exitp < 0 ? makeMoveImpl(view) : moveToPos(view, exitp);

            _dir = (_dir + move) % 4;
            updatePos(_posr,_posc,_dir);
            return move;
        }
    };

    struct FilememAgent : Agent
    {
        int makeMove(const Area &view) override
        {
            return 0;
        }
    };

    Agent* Agent::create(const char *type)
    {
        if (0==strcmp(type,"nomem_1")) { return new MemorylessAgent1(); }
        if (0==strcmp(type,"nomem_2")) { return new MemorylessAgent2(); }
        if (0==strcmp(type,"mem")) { return new MemoryAgent(); }
        if (0==strcmp(type,"filemem")) { return new FilememAgent(); }
        return new MemorylessAgent1();
    }
}