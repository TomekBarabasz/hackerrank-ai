//
// Created by barabasz on 07.06.2021.
//

#include <vector>
#include <string>
#include <map>
#include "maze-escape.h"

using namespace std;
namespace MazeEscape
{
    string rot90(const string &area, bool cw)
    {
        static const int iccw[] = {2, 5, 8, 1, 4, 7, 0, 3, 6};
        static const int icw[] = {6,3,0,7,4,1,8,5,2};
        char tmp[10];
        char *p = tmp;
        for (int i : cw ? icw : iccw) {
            *p++ = area[i];
        }
        *p = '\0';
        return string(tmp);
    }

    void Area::init()
    {
        area_ = {"???", "???", "???"};
        _rows= _cols=3;
    }

    void Area::extend(int posr, int posc)
    {
        if (posr-1 < topr) {
            //extend NORTH
            string row(_cols, '?');
            area_.insert(area_.begin(), row);
            topr--; _rows++;
        }else if (posr+1 >= topr + _rows) {
            //extend SOUTH
            string row(_cols, '?');
            area_.push_back(row);
            topr; _rows++;
        }
        if (posc-1 < topc) {
            //EXTEND WEST
            for (auto & r : area_) {
                r = string(1,'?') + r;
            }
            topc--;_cols++;
        }else if (posc+1 >= topc + _cols) {
            //EXTEND EAST
            for (auto & r : area_){
                r = r + '?';
            }
            _cols++;
        }
    }
    //note: _posr, _posc are center of view
    void Area::append(const Area &view, int posr, int posc, char visited)
    {
        if (!area_.empty()) extend(posr, posc);
        else init();
        posr--; posc--;
        //_posr,_posc are now upper left corner
        const int ridx = posr - topr;
        const int cidx = posc - topc;

        for (int r=0;r<3;++r) {
            for (int c=0;c<3;++c){
                char & pos = area_[ridx + r][cidx + c];
                if (pos != visited) {
                    pos = (r!=1 || c!=1 ? view[r][c] : visited);
                }
            }
        }
    }
    Area Area::getView(int posr, int posc, int dir) const
    {
        struct Point {
            int row, col;
        };
        static Point up[] = {{-1, -1},
                             {-1, 0},
                             {-1, 1},
                             {0,  -1},
                             {0,  0},
                             {0,  1},
                             {1,  -1},
                             {1,  0},
                             {1,  1}};
        static Point right[] = {{-1, 1},
                                {0,  1},
                                {1,  1},
                                {-1, 0},
                                {0,  0},
                                {1,  0},
                                {-1, -1},
                                {0,  -1},
                                {1,  -1}};
        static Point down[] = {{1,  1},
                               {1,  0},
                               {1,  -1},
                               {0,  1},
                               {0,  0},
                               {0,  -1},
                               {-1, 1},
                               {-1, 0},
                               {-1, -1}};
        static Point left[] = {{1,  -1},
                               {0,  -1},
                               {-1, -1},
                               {1,  0},
                               {0,  0},
                               {-1, 0},
                               {1,  1},
                               {0,  1},
                               {-1, 1}};
        static const map<int, Point *> Offset =
                {
                        {0,/*UP*/    up},
                        {1, /*RIGHT*/right},
                        {2, /*DOWN*/ down},
                        {3, /*LEFT*/ left}
                };
        const Point *offset = Offset.at(dir);
        vector<string> view;
        string row;
        posr -= topr;
        posc -= topc;
        for (int i = 0; i < 9; ++i, offset++) {
            row += area_[posr + offset->row][posc + offset->col];
            if (row.size() == 3) {
                view.push_back(row);
                row = "";
            }
        }
        return view;
    }
    Area Area::alignNorth(int dir) const
    {
        string a = concat();
        while(dir != 0)
        {
            a = rot90(a,false);
            dir = (dir + 1) % 4;
        }
        return Area(a);
    }
    int Area::find(char element) const
    {
        int position = 0;
        for (const auto &r : area_) {
            auto p = r.find(element);
            if (p != string::npos) {
                return position + p;
            }
            position += r.size();
        }
        return -1;
    }
    bool Area::compare(char exp, char c)
    {
        if (UNKNOWN_TILE == exp) return true;
        if (exp == c) return true;
        if (exp=='-' && c=='.') return true;
        return false;
    }
    Direction Area::matchFeature(string feature, bool rot) const
    {
        auto area = concat();
        for (int i = 0; i < 4; ++i) {
            //if (area == feature) return i;
            bool same=true;
            for (int j=0;j<area.size();++j) {
                if (!compare(feature[j], area[j])) {
                    same=false;
                    break;
                }
            }
            if (same) return static_cast<Direction>(i);
            if (!rot) break;
            feature = rot90(feature, true);
        }
        return Direction::INVALID;
    }
    void Area::dump() const
    {
        for (auto & r : area_) std::cout << r << std::endl;
    }

}