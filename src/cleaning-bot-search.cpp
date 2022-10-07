#include<iostream>
#include<vector>
#include <algorithm>
#include <time.h>
#include <cstring>
#include "lib/board.h"
#include "lib/utils.h"

using namespace std;
namespace {
void next_move_searchtree(int pos, int N, const vector<string> &board)
{
}

void next_move(int posr, int posc, const vector<string> &board)
{
    const int N = board[0].size();
    const int pos = posr * N + posc;
    if (board[posr][posc] == 'd') cout << "CLEAN" << endl;
    else next_move_searchtree(pos, N, board);
}

int cleaning_bot_search_test(int argc, char **argv)
{
    if (argc < 3){
        cout << "arguments : Width Height Density [Niter]" << endl;
        return 1;
    }
    srand(time(NULL));
    const int W = atoi(argv[0]);
    const int H = atoi(argv[1]);
    const int Density = atoi(argv[2]);
    int Niter = argc > 3 ? atoi(argv[3]) : 1;
    int ci = 1;
    while (Niter--) {
        auto board = CBot::makeRandomBoard(H, W, Density);
        cout << "testing board " << ci++ << endl;
        for (auto &r : board) {
            cout << r << endl;
        }
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c)
                next_move(r, c, board);
        }
    }
    return 0;
}

int cleaning_bot_search_solve(int argc, char **argv)
{
    int pos[2];
    vector<string> board;
    int N = atoi(argv[1]);
    pos[0] = atoi(argv[2]);
    pos[1] = atoi(argv[3]);
    string sboard(argv[4]);

    cout << "pos " << pos[0] << " " << pos[1] << " N " << N << endl;
    for (int r = 0; r < N * N; r += N) {
        board.push_back(sboard.substr(r, N));
    }
    for (auto &r : board) {
        cout << r << endl;
    }
    next_move(pos[0], pos[1], board);
    return 0;
}
}
int cleaning_bot_search_main(int argc, char**argv)
{
    if (argc >=1 )
    {
        if (0 == strcmp(argv[0], "test"))  return cleaning_bot_search_test(argc-1, argv+1);
        if (0 == strcmp(argv[0], "solve")) return cleaning_bot_search_solve(argc-1, argv+1);
    }else{
        cout << "available options are : [test|solve]" << endl;
    }
    return 1;
}
