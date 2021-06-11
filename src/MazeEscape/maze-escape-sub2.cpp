#include <cmath>
#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

string dirToString(int dir)
{
    static const map<int, string> Dirs =
            {
                    {0,"UP"}, {1, "RIGHT"}, {2, "DOWN"}, {3, "LEFT"}
            };
    return Dirs.at(dir);
}
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
enum Direction { UP=0, RIGHT=1, DOWN=2, LEFT=3, INVALID=-1 };

struct Area
{
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
    Direction matchFeature(string feature) const;

    int toprow()  const { return topr; }
    int leftcol() const { return topc; }
    bool isWall(int r, int c) const { return area_[r][c] == WALL_TILE; }
    bool isVisited(int r, int c) const { return area_[r][c] == VISITED_TILE; }

    void dump() const;
    static constexpr char UNKNOWN_TILE = 'x';
    static constexpr char WALL_TILE = '#';
    static constexpr char VISITED_TILE = '.';

private:
    void extend(int posr, int posc);
    vector<string> area_;
    int topr, topc, _rows, _cols;
};
inline Area rot90cw(const Area& a)  { return Area(rot90(a.concat(),true)); }
inline Area rot90ccw(const Area& a) { return Area(rot90(a.concat(),false)); }
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
Direction Area::matchFeature(string feature) const
{
    auto area = concat();
    for (int j=0;j<area.size();++j) {
        if (!compare(feature[j], area[j])) {
            return Direction::INVALID;
        }
    }
    return Direction::UP;
}
struct Feature { string str; int dir; };
using FeatureList_t = vector<Feature>;
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
Direction tryMoveByFeatureEx(const Area& a, const FeatureList_t & features)
{
    for (auto & f : features) {
        int v = a.matchFeature(f.str);
        if (v != Direction::INVALID) {
            return static_cast<Direction>((f.dir + v) % 4);
        }
    }
    return Direction::INVALID;
}
template <typename T> T oneOf(T a, T b) { return rand() < RAND_MAX / 2 ? a : b; }
Direction makeRandomMove(const Area &view)
{
    vector<Direction> dirs;
    if (view[0][1] == '-') dirs.push_back(Direction::UP);
    if (view[1][0] == '-') dirs.push_back(Direction::LEFT);
    if (view[2][1] == '-') dirs.push_back(Direction::DOWN);
    if (view[1][2] == '-') dirs.push_back(Direction::RIGHT);
    return dirs[rand() % dirs.size()];
}
Direction moveToPos(const Area &view, int pos)
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
            return makeRandomMove(view);
    }
}
Area parseInput()
{
    int playerId;
    cin>>playerId;
    vector<string> visible_area;
    for (int i=0; i<3;++i){
        string s;
        cin>>s;
        visible_area.push_back(s);
    }
    Area view(visible_area);
    return view;
}
int main()
{
    Area view = parseInput();

    const int exitp = view.find('e');
    int move;
    if (exitp < 0)
    {
        const auto dir = tryMoveByFeatureEx(view, stick_to_wall);
        if(dir != Direction::INVALID) move=dir;
        else {move=makeRandomMove(view); cout << "random move ";}
    }
    else
    {
        move = moveToPos(view, exitp);
    }

    cout << dirToString( move ) << endl;

    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    return 0;
}
