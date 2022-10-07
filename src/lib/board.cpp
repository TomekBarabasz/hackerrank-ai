//
// Created by barabasz on 05.05.2021.
//
#include "board.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>

using namespace std;

namespace CBot
{
vector<string> makeRandomBoard(int h, int w, int dnum)
{
    const int HW = h * w;
    set<int> dpos;
    vector<string> board;

    while (dpos.size() < dnum) {
        dpos.insert(rand() % HW);
    }
    //cout << "board pos ";
    //for (auto p : dpos) cout << p << " ";
    //cout << endl;
    ostringstream ss;
    int pos = 0;
    auto ipos = dpos.begin();
    while (h--) {
        for (int i = 0; i < w; ++i, ++pos) {
            if (pos == *dpos.begin()) {
                ss << "d";
                dpos.erase(dpos.begin());
            }
            else ss << "-";
        }
        board.push_back(ss.str());
        ss.clear();
        ss.str("");
    }
    return board;
}

std::tuple<int,int> makeRandomPosition(int rows, int cols)
{
    return { rand() % rows, rand() % cols };
}

vector<int> findDirt(const vector<string> &board)
{
    vector<int> dpos;
    int pos = 0;
    for (auto &row : board) {
        for (int c = 0; c < row.size(); ++c, ++pos) {
            if (row[c] == 'd') dpos.push_back(pos);
        }
    }
    return dpos;
}

int calcSteps(int p0, int p1, int N)
{
    const int p0x = p0 % N;
    const int p0y = p0 / N;
    const int p1x = p1 % N;
    const int p1y = p1 / N;
    return abs(p0x - p1x) + abs(p0y - p1y);
}

int evalRoute(int start, const vector<int> &route, int n)
{
    int totsteps = 0;
    for (auto pos : route) {
        totsteps += calcSteps(start, pos, n);
        start = pos;
    }
    return totsteps;
}
}

namespace Pac
{
    Board::Neighbours_t::Neighbours_t(std::initializer_list<FieldId_t> init)
    {
        cnt=0;
        for (auto f : init) ids[cnt++]=f;
    }
    Board::Fields_t scanFields(const vector<string>& desc, char open, char blocked)
    {
        Board::Fields_t fields;
        const unsigned int WIDTH = desc[0].size();
        for (int row=0; row < desc.size(); ++row) {
            //cout << desc[row] << endl;
            for (int col=0;col<WIDTH;++col) {
                if (desc[row][col] == open){
                    fields.insert(Board::pos2id(row, col) );
                    //cout << row << " " << col << " " << desc[row][col] << endl;
                }
            }
        }
        return fields;
    }

    Board Board::create(const vector<string>& desc, char open, char blocked)
    {
        Board board;

        board.WIDTH = desc[0].size();
        board.HEIGHT= desc.size();

        board.fields = scanFields(desc, open, blocked);
        for (const auto & f : board.fields) {
            board.graph[f] = board.makeNeighbours(f);
        }
        return board;
    }
    void Board::dump()
    {
        cout << "fields" << endl;
        for (const auto & f : fields) {
            cout << id2s(f) << endl;
        }
        cout << "edges" << endl;
        for(const auto & kv : graph) {
            cout << id2s(kv.first) << ":";
            const auto & ne = kv.second;
            for (int i=0;i<ne.cnt; ++i) cout << id2s(ne.ids[i]) << ",";
            cout << endl;
        }
    }
    string Board::id2s(Board::FieldId_t f)
    {
        int r,c;
        Board::id2pos(f,r,c);
        return "(" + std::to_string(r) + "," + std::to_string(c) + ")";
    }
    Board::Neighbours_t Board::makeNeighbours(Board::FieldId_t f)
    {
        int r,c;
        Neighbours_t n;
        id2pos(f,r,c);
        if (r<HEIGHT-1){ //DOWN
            auto ne = pos2id(r+1,c);
            if(fields.find(ne)!=fields.end()){
                n.add(ne);
            }
        }
        if (c<WIDTH-1) { //RIGHT
            auto ne = pos2id(r,c+1);
            if (fields.find(ne) != fields.end()) {
                n.add(ne);
            }
        }
        if (c>0) { //LEFT
            auto ne = pos2id(r,c-1);
            if (fields.find(ne)!=fields.end()){
                n.add(ne);
            }
        }
        if (r>0) { //UP
            auto ne = pos2id(r-1,c);
            if (fields.find(ne)!=fields.end()){
                n.add( ne );
            }
        }
        return n;
    }
    void dumpPath(const vector<Board::FieldId_t>& path)
    {
        for(const auto & e: path){
            cout << Board::id2s(e) << " ";
        }
        cout << endl;
    }
    vector<Board::FieldId_t> Board::findPath(Board::FieldId_t from, Board::FieldId_t to)
    {
        int shortestDist = 1000000;
        vector<Board::FieldId_t> path {from}, shortestPath;
        vector<Neighbours_t> queue{ graph[from] };

        while( !queue.empty() )
        {
            auto & neighbours = queue.back();
            auto next_field = path.back();
            bool found=false;

            while (neighbours.cnt > 0 && !found)
            {
                next_field = neighbours.ids[--neighbours.cnt];
                if (std::find(path.begin(), path.end(), next_field) == path.end()) {
                    found = true;
                }
            }

            if (found)
            {
                path.push_back(next_field);
                queue.push_back(graph[next_field]);

                if (next_field==to)
                {
                    const int dist = path.size();
                    //cout << "new path found, length " << dist << endl;
                    //dumpPath(path);
                    if (dist < shortestDist) {
                        //cout << "replacing shortest path" << endl;
                        shortestDist = dist;
                        shortestPath = path;
                    }
                }
                else if (path.size() >= shortestDist) {
                    //no need to continue, backtrack!
                }
                else {
                    continue;
                }
            }
            queue.pop_back();
            path.pop_back();
        }

        return shortestPath;
    }
}