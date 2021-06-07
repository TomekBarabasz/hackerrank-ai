#include<iostream>
#include<vector>
#include <functional>
#include <limits>
#include <algorithm>
#include <lib/board.h>
#include <lib/utils.h>

using namespace std;
using namespace Utils;
using CBot::makeRandomBoard;
using CBot::makeRandomPosition;

vector<int> findDirt(const vector<string> & board)
{
    vector<int> dpos;
    int pos=0;
    for (auto & row : board){
        for(int c=0; c<row.size(); ++c, ++pos) {
            if (row[c]=='d') dpos.push_back(pos);
        }
    }
    return dpos;
}
int calcSteps(int p0, int p1, int N)
{
    const int p0x = p0%N;
    const int p0y = p0/N;
    const int p1x = p1%N;
    const int p1y = p1/N;
    return abs(p0x-p1x) + abs(p0y-p1y);
}
int evalRoute(int start, const vector<int> & route, int rowLength)
{
    int totsteps = 0;
    for (auto pos : route){
        totsteps += calcSteps(start, pos, rowLength);
        start = pos;
    }
    return totsteps;
}
void printMoves(int pos, int dest, int dimh, int dimw, int cnt)
{
    const string commands[] = {"LEFT", "RIGHT", "UP", "DOWN"};
    const int dx = dest%dimw - pos%dimw;
    const int dy = (dest-pos) / dimw;
    if (dx != 0){
        int adx = min(cnt,abs(dx));
        const int dir = dx < 0 ? 0 : 1;
        while(adx--) {cout << commands[dir] << endl;--cnt;}
    }
    if (dy != 0){
        int ady = min(cnt,abs(dy));
        const int dir = dy < 0 ? 2 : 3;
        while(ady--) cout << commands[dir] << endl;
    }
}
void next_move_permutations(int pos, int dimh, int dimw, const vector <string> & board)
{
    const int rowLength = board[0].size();
    vector<int> dirtPos = findDirt(board);
    vector<int> shortesRoute;
    int shortesDistance=numeric_limits<int>::max();

    auto eval = [&](const vector<int>& route) {
        const int d = evalRoute(pos, route, rowLength);
        if (d < shortesDistance){
            shortesDistance = d;
            shortesRoute = route;
        }
    };
    if (dirtPos.size() > 7) {
        int niter=5000;
        while(niter--) {
            std::random_shuffle(dirtPos.begin(), dirtPos.end());
            eval(dirtPos);
        }
    }
    else if (dirtPos.size() > 1) {
        while(std::next_permutation(dirtPos.begin(), dirtPos.end())) {
            eval(dirtPos);
        }
    }else{
        eval(dirtPos);
    }
    printMoves(pos, shortesRoute[0], dimh, dimw, 1);
}
void next_move(int posr, int posc, int dimh, int dimw, const vector <string> & board)
{
    const int pos = posr * dimw + posc;
    if (board[posr][posc] == 'd') cout << "CLEAN" << endl;
    else next_move_permutations(pos,dimh,dimw,board);
}

vector<int> findShortestPath(const vector<string> &board, int posr, int posc)
{
    const int COLS = board[0].size();
    auto dirtPos = findDirt(board);
    return {};
}

int main(int argc, char** argv)
{
    const int ROWS = atoi(argv[1]);
    const int COLS = atoi(argv[2]);
    const int Density = argc > 3 ? atoi(argv[3]) : 30;
    int NiterPos = argc > 4 ? atoi(argv[4]) : 1;
    int NiterBoard = argc > 5 ? atoi(argv[5]) : 1;

    while(NiterBoard--)
    {
        auto board = makeRandomBoard(ROWS,COLS,Density);
        printBoard(board);
        for (int ip=0;ip<NiterPos;++ip)
        {
            auto [posr,posc] = makeRandomPosition(ROWS,COLS);
            std::cout << "posr " << posr << " posc " << posc << endl;
            auto path = findShortestPath(board, posr, posc);
        }
    }

    return 0;
}


