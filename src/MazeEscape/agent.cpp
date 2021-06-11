//
// Created by barabasz on 07.06.2021.
//
#include <cstring>
#include <map>
#include <tuple>
#include "maze-escape.h"

using std::map;
using std::string;
using std::tuple;

namespace MazeEscape
{
    template <typename T> T oneOf(T a, T b) { return rand() < RAND_MAX / 2 ? a : b; }
    Direction makeRandomDir() { return static_cast<Direction>( rand() % 4 ); }
    Direction dirRot90cw(Direction dir) { return static_cast<Direction>( (dir+1) % 4); }
    Direction Agent::moveToPos(const Area &view, int pos)
    {
        switch (pos) {
            case 0:
                if (view[0][1] == '-') return Direction::UP;
                if (view[1][0] == '-') return Direction::LEFT;
                return oneOf(Direction::RIGHT, Direction::DOWN);
            case 1:
                return Direction::UP;
            case 2:
                if (view[0][1] == '-') return Direction::UP;
                if (view[1][2] == '-') return Direction::RIGHT;
                return oneOf(Direction::DOWN, Direction::LEFT);
            case 3:
                return Direction::LEFT;
            case 5:
                return Direction::RIGHT;
            case 6:
                if (view[1][0] == '-') return Direction::LEFT;
                if (view[2][1] == '-') return Direction::DOWN;
                return oneOf(Direction::UP, Direction::RIGHT);
            case 7:
                return Direction::DOWN;
            case 8:
                if (view[1][2] == '-') return Direction::RIGHT;
                if (view[2][1] == '-') return Direction::DOWN;
                return oneOf(Direction::LEFT, Direction::UP);
            default:
                return makeRandomDir();
        }
    }
    Direction makeRandomMove(const Area &view)
    {
        vector<Direction> dirs;
        if (view[0][1] == '-') dirs.push_back(Direction::UP);
        if (view[1][0] == '-') dirs.push_back(Direction::LEFT);
        if (view[2][1] == '-') dirs.push_back(Direction::DOWN);
        if (view[1][2] == '-') dirs.push_back(Direction::RIGHT);
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
    FeatureList_t stick_to_wall = {
        {"#-x"\
         "#xx"\
         "#xx", Direction::UP},
        {"x-#"\
         "xx#"\
         "xx#", Direction::UP},
        {"xxx"\
         "-xx"\
         "#xx", Direction::LEFT},
        {"xxx"\
         "xx-"\
         "xx#", Direction::RIGHT},
        {"x-x"\
         "xxx"\
         "xxx", Direction::UP},
        {"x#x"\
         "-xx"\
         "xxx", Direction::LEFT},
         {"x#x"\
         "xx-"\
         "xxx", Direction::RIGHT}
    };
    FeatureList_t up_right_left_back = {
        {"x-x"\
         "xxx"\
         "xxx", Direction::UP},
         {"x#x"\
         "-xx"\
         "xxx", Direction::LEFT},
         {"x#x"\
         "xx-"\
         "xxx", Direction::RIGHT},
         {"x#x"\
         "#x#"\
         "x-x", Direction::DOWN}
    };
    Direction Agent::tryMoveByFeature(const Area& a)
    {
        return tryMoveByFeatureEx(a, stick_to_wall);
    }
    Direction Agent::tryMoveByFeatureEx(const Area& a, const FeatureList_t & features)
    {
        for (auto & [str,dir] : features) {
            int v = a.matchFeature(str, false);
            if (v!=Direction::INVALID) return static_cast<Direction>((dir+v) % 4);
        }
        return Direction::INVALID;
    }
    struct MemorylessAgent : Agent
    {
        Direction makeMove(const Area &view) override
        {
            const int exitp = view.find('e');
            if (exitp < 0)
            {
                auto dir = tryMoveByFeatureEx(view, stick_to_wall);
                if (dir != Direction::INVALID) {
                    return dir;
                }
                std::cout << "position " << _posr << "," << _posc << " agent doing random move!" << std::endl;
                view.dump();
                return makeRandomMove(view);
            }
            else
            {
                return moveToPos(view, exitp);
            }
        }
    };

    struct MemoryAgent : Agent
    {
        Direction makeMoveImpl(const Area &area)
        {
            auto view = area.getView(_posr,_posc,_dir);
            auto dir = tryMoveByFeatureEx(view, stick_to_wall);
            if (dir != Direction::INVALID) {
                return dir;
            }
            std::cout << "position " << _posr << "," << _posc << " agent doing random move!" << std::endl;
            area.dump();
            return makeRandomMove(view);
        }

        Direction makeMove(const Area &view) override
        {
            Area aligned = view.alignNorth(_dir);
            exploredArea.append(aligned, _posr, _posc, Area::VISITED_TILE);

            const int exitp = view.find('e');
            auto move = exitp < 0 ? makeMoveImpl(exploredArea) : moveToPos(view, exitp);

            _dir = (_dir + move) % 4;
            updatePos(_posr,_posc,_dir);
            return move;
        }
    };

    struct FilememAgent : Agent
    {
        Direction makeMove(const Area &view) override
        {
            return makeRandomDir();
        }
    };

    Agent* Agent::create(const char *type)
    {
        if (0==strcmp(type,"nomem")) { return new MemorylessAgent(); }
        if (0==strcmp(type,"mem")) { return new MemoryAgent(); }
        if (0==strcmp(type,"filemem")) { return new FilememAgent(); }
        return new MemorylessAgent();
    }
}