//
// Created by barabasz on 07.06.2021.
//

#include <vector>
#include <string>
#include <map>
#include <fstream>
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
    Area::Area(std::ifstream& in)
    {
        load(in);
    }

    void Area::init()
    {
        string row(3,UNKNOWN_TILE);
        area_={row,row,row};
        _rows= _cols=3;
    }

    void Area::extend(int posr, int posc)
    {
        if (posr-1 < topr) {
            //extend NORTH
            string row(_cols, UNKNOWN_TILE);
            area_.insert(area_.begin(), row);
            topr--; _rows++;
        }else if (posr+1 >= topr + _rows) {
            //extend SOUTH
            string row(_cols, UNKNOWN_TILE);
            area_.push_back(row);
            topr; _rows++;
        }
        if (posc-1 < topc) {
            //EXTEND WEST
            for (auto & r : area_) {
                r = string(1,UNKNOWN_TILE) + r;
            }
            topc--;_cols++;
        }else if (posc+1 >= topc + _cols) {
            //EXTEND EAST
            for (auto & r : area_){
                r = r + UNKNOWN_TILE;
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
    Area Area::transform(const std::map<char,char>& trans)
    {
        for (auto & row : area_)
        {
            for (auto & c : row) {
                auto it = trans.find(c);
                if (it != trans.end()) {
                    c = it->second;
                }
            }
        }
    }
    bool Area::load(std::ifstream& in)
    {
        in >> topr >> topc >> _rows >> _cols;
        string row;
        for (int i=0;i<_rows;++i){
            in >> row;
            area_.push_back(row);
        }
    }
    bool Area::store(std::ofstream& out)
    {
        out << topr << endl << topc << endl << _rows << endl << _cols << endl;
        for (const auto & row : area_) {
            out << row << endl;
        }
    }
    template<typename T>
    tuple<T,bool> min(T value, T limit)
    {
        if (value < limit) return {value,false};
        else return {limit,true};
    }
    template<typename T>
    tuple<T,bool> max(T value, T limit)
    {
        if (value > limit) return {value,false};
        else return {limit,true};
    }

    Area::Locations_t Area::findClosest(int origr, int origc, char element) const
    {
        Locations_t pts;

        for(int dist=1;pts.empty();++dist)
        {
            const auto [rmin,rminclamped] = max(origr-dist, topr);
            const auto [rmax,rmaxclamped] = min(origr+dist, topr+_rows-1);
            const auto [cmin,cminclamped] = max(origc-dist, topc);
            const auto [cmax,cmaxclamped] = min(origc+dist, topc+_cols-1);
            if (rminclamped && rmaxclamped && cminclamped && cmaxclamped) break;
            if (!rminclamped){
                for (int c=cmin; c<cmax;++c) if (area_[rmin][c]==element) pts.push_back({rmin,c,Direction::UP});
            }
            if (!rmaxclamped){
                for (int c=cmin+1; c<=cmax;++c) if (area_[rmax][c]==element) pts.push_back({rmax,c,Direction::DOWN});
            }
            if (!cminclamped){
                for (int r=rmin+1; r<=rmax;++r) if (area_[r][cmin]==element) pts.push_back({r,cmin,Direction::LEFT});
            }
            if (!cmaxclamped){
                for (int r=rmin; r<rmax;++r) if (area_[r][cmax]==element) pts.push_back({r,cmax,Direction::RIGHT});
            }
            if (!pts.empty()) break;
        }
        return pts;
    }
    bool Area::compare(int origr, int origc, const Area& view) const
    {
        for (int c=0; c<view.cols(); ++c)
            for (int r=0;r<view.rows(); ++r)
                if (area_[origr + r][origc + c] != view.area_[r][c]) return false;
        return true;
    }
    Area::Locations_t Area::findPattern(const Area& view) const
    {
        Area r0 = view;
        Area r90 = MazeEscape::rot90cw(view);
        Area r180 = MazeEscape::rot90cw(r90);
        Area r270 = MazeEscape::rot90cw(r180);
        Area* rotated[] = { &r0, &r90, &r180, &r270 };

        Locations_t result;
        const int max_col = _cols-view.cols();
        const int max_row = _rows-view.rows();
        for (int row=0; row <= max_row; ++row) {
            for (int col=0; col <= max_col; ++col) {
                for (int dir=0;dir<4;++dir) {
                    if (compare(row,col,*rotated[dir])) { result.push_back( {row,col,dir} );}
                }
            }
        }

        return result;
    }
    void Area::set(int posr, int posc, char element)
    {
        area_[posr][posc] = element;
    }
}