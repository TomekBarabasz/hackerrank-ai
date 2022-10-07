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

int findElement(const vector<string>& area, int element)
{
    int position=0;
    for (const auto & r : area){
        auto p = r.find(element);
        if (p != string::npos){
            return position + p;
        }
        position += r.size();
    }
    return -1;
}

inline int oneOf(int a, int b)
{
    return rand() < RAND_MAX/2 ? a : b;
}
int moveToPos(const vector<string>& area, int pos)
{
    switch(pos)
    {
        case 0:
            if (area[0][1]=='-') return 0;
            if (area[1][0]=='-') return 3;
            return oneOf(1,2);
        case 1: return 0;
        case 2:
            if (area[0][1]=='-') return 0;
            if (area[1][2]=='-') return 1;
            return oneOf(2,3);
        case 3: return 3;
        case 5: return 1;
        case 6:
            if (area[1][0]=='-') return 3;
            if (area[2][1]=='-') return 2;
            return oneOf(0,1);
        case 7: return 2;
        case 8:
            if (area[1][2]=='-') return 1;
            if (area[2][1]=='-') return 2;
            return oneOf(3,0);
        default:
            return rand()%4;
    }
}
int makeRandomMove(const vector<string>& area)
{
    vector<int> dirs;
    if (area[0][1]=='-') dirs.push_back(0);
    if (area[1][0]=='-') dirs.push_back(1);
    if (area[2][1]=='-') dirs.push_back(2);
    if (area[1][2]=='-') dirs.push_back(3);
    return dirs[ rand() % dirs.size() ];
}
string rot90(const string &area, bool cw=true)
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
int matchFeature(const string& area, string feature)
{
    for (int i=0;i<4;i++){
        if (area == feature) return i;
        feature = rot90(feature);
    }
    return -1;
}
int goAlongAwall(const vector<string>& area)
{
    string a = area[0]+area[1]+area[2];
    const string wall_front = "###------";
    int dir;
    dir = matchFeature(a, wall_front);
    if (dir >=0) return (dir+1)%4;

    const string corner_left = "####--#--";
    dir = matchFeature(a, corner_left);
    if (dir >=0) return (dir+1)%4;

    if (area[0][1]=='-') return 0;
    if (area[0][0]=='-') return 3;
    if (area[0][2]=='-') return 1;

    return rand() % 4;
}
void read_area()
{
    /*cout << "area:" << endl;
    for (const auto & r : visible_area){
        cout << r << endl;
    }
    cout << endl;*/

    /*{ifstream tmp("myfile.txt");
    if (tmp.is_open()){
        string line;
        tmp >> line;
        cout << "line from file " << line << endl;
        tmp.close();
    }}
    {ofstream tmp("myfile.txt");
    tmp << 123 << endl;
    }*/
}
int main()
{
    int playerId;
    cin>>playerId;
    vector<string> visible_area;
    for (int i=0; i<3;++i){
        string s;
        cin>>s;
        visible_area.push_back(s);
    }

    const int exitp = findElement(visible_area,'e');
    int move = -1;
    if (exitp < 0)
    {
        move = goAlongAwall(visible_area);
    }
    else
    {
        move = moveToPos(visible_area,exitp);
    }
    cout << dirToString( move ) << endl;

    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    return 0;
}
