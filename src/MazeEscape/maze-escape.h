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
using std::vector;
using std::string;
using std::ofstream;

namespace MazeEscape
{
    inline int oneOf(int a, int b) { return rand() < RAND_MAX / 2 ? a : b; }
    string rot90(const string &area, bool cw);
    struct Area
    {
        Area() : topr(-1), topc(-1), _rows(0), _cols(0){}
        Area(const string& area) : area_(Area::split(area)), topr(-1), topc(-1), _rows(3), _cols(3){}
        Area(const vector<string>& area) : area_(area), topr(-1), topc(-1), _rows(area.size()), _cols(area[0].size()){}

        void init();
        void append(const Area &view, int posr, int posc, int dir, char visited='-');
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
        Area alingNorth(int dir) const;
        int find(char element) const;
        int matchFeature(string feature) const;

        int toprow()  const { return topr; }
        int leftcol() const { return topc; }

    private:
        void extend(int posr, int posc);

        static constexpr char UNKNOWN = '?';
        vector<string> area_;
        int topr, topc, _rows, _cols;
    };
    inline Area rot90cw(const Area& a)  { return Area(rot90(a.concat(),true)); }
    inline Area rot90ccw(const Area& a) { return Area(rot90(a.concat(),false)); }

    struct Agent
    {
        int makeMove(const Area &view, ofstream &storage);
        int goAlongAwall(const Area &area);
        int moveToPos(const Area &view, int pos);
        int makeRandomMove(const Area &view);
    };
}

#endif //HACKERRANK_AI_MAZE_ESCAPE_H
