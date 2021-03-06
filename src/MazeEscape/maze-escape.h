//
// Created by barabasz on 20.05.2021.
//

#ifndef HACKERRANK_AI_MAZE_ESCAPE_H
#define HACKERRANK_AI_MAZE_ESCAPE_H
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <map>
#include <tuple>

using std::vector;
using std::string;
using std::ofstream;
using std::tuple;

namespace MazeEscape
{
    string rot90(const string &area, bool cw);
    enum Direction { UP=0, RIGHT=1, DOWN=2, LEFT=3, INVALID=-1 };

    struct Area
    {
        static constexpr char UNKNOWN_TILE = 'x';
        static constexpr char WALL_TILE = '#';
        static constexpr char VISITED_TILE = '.';

        Area(std::ifstream&);
        Area(int topr_=0, int topc_=0) : topr(topr_-1), topc(topc_-1), _rows(0), _cols(0){}
        Area(const string& area) : area_(Area::split(area)), topr(-1), topc(-1), _rows(3), _cols(3){}
        Area(const vector<string>& area) : area_(area), topr(0), topc(0), _rows(area.size()), _cols(area[0].size()){}

        void init();
        void append(const Area &view, int posr, int posc, char visited='-');
        const string & getRow(int idx) const { return area_[idx];}
        int cols() const { return _cols;}
        int rows() const { return _rows;}
        Area getView(int posr, int posc, int dir) const;

        string& operator[](int i) { return area_[i];}
        const string& operator[](int i) const { return area_[i];}
        void rot90cw()  { area_ = split(rot90(concat(), true )); }
        void rot90ccw() { area_ = split(rot90(concat(), false)); }
        string concat() const { return area_[0] + area_[1] + area_[2]; }
        static vector<string> split(const string& area) { return { area.substr(0,3), area.substr(3,3), area.substr(6,3) }; }
        Area alignNorth(int dir) const;
        int find(char element) const;
        static bool compare(char exp, char c);
        Direction matchFeature(string feature, bool rot) const;
        struct Location { int posr; int posc; int dir;};
        using Locations_t = vector<Location>;
        Locations_t findPattern(const Area&) const;
        Area::Locations_t findClosest(int origr, int origc, char element) const;
        bool compare(int origr, int origc, const Area&) const;
        int toprow()  const { return topr; }
        int leftcol() const { return topc; }
        bool isWall(int r, int c) const { return area_[r][c] == WALL_TILE; }
        bool isVisited(int r, int c) const { return area_[r][c] == VISITED_TILE; }
        void set(int posr, int posc, char element);
        void dump() const;
        Area transform(const std::map<char,char>&);
        bool load(std::ifstream&);
        bool store(std::ofstream&);

    private:
        void extend(int posr, int posc);
        vector<string> area_;
        int topr, topc, _rows, _cols;
    };
    inline Area rot90cw(const Area& a)  { return Area(rot90(a.concat(),true)); }
    inline Area rot90ccw(const Area& a) { return Area(rot90(a.concat(),false)); }

    using FeatureList_t = vector<tuple<string,Direction>>;
    struct Agent
    {
        static Agent* create(const char* type);
        Agent() : _posr(0), _posc(0), _dir(0) {}

        virtual Direction makeMove(const Area &view)=0;
        Direction moveToPos(const Area &view, int pos);

        int row() const { return _posr;}
        int col() const { return _posc;}
        int dir() const { return _dir; }

        const Area& getExploredArea() const { return exploredArea; }
        static void updatePos(int& posr,int& posc, int dir);
        static Direction tryMoveByFeature(const Area& a);
        static Direction tryMoveByFeatureEx(const Area& a, const FeatureList_t&);
    protected:
        int _posr,_posc,_dir;
        Area exploredArea;
    };
}

#endif //HACKERRANK_AI_MAZE_ESCAPE_H
