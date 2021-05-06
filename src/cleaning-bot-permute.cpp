#include<iostream>
#include<vector>
#include <functional>
#include <limits>
#include <algorithm>
#include <cstring>
#include "board.h"
#include "utils.h"

using namespace std;

namespace {
void swap(int &a, int &b) {
    const int tmp = a;
    a = b;
    b = tmp;
}

void generatePermutations(vector<int> &perm, int i, int n, std::function<void(const vector<int> &)> eval) {
    if (i == n) eval(perm);
    else {
        for (int j = i; j <= n; ++j) {
            swap(perm[i], perm[j]);
            generatePermutations(perm, i + 1, n, eval);
            swap(perm[i], perm[j]);
        }
    }
}

void next_move_permutations(int pos, int N, const vector<string> &board) {
    vector<int> dirtPos = CBot::findDirt(board);
    vector<int> shortesRoute;
    int shortesDistance = numeric_limits<int>::max();
    int numChecked = 0;
    auto eval = [&](const vector<int> &route) {
        const int d = CBot::evalRoute(pos, route, N);
        //cout << "route ";
        //for (auto p : route) cout << p << " ";
        //cout << " steps " << d;
        if (d < shortesDistance) {
            shortesDistance = d;
            shortesRoute = route;
            //cout << " best";
        }
        //cout << endl;
        if (0 == (++numChecked % 5000)) cout << ".";
    };
    cout << "found " << dirtPos.size() << " dirt positions, checking permuations ";

    //generatePermutations(dirtPos, 0, dirtPos.size()-1, eval);
    while (std::next_permutation(dirtPos.begin(), dirtPos.end())) {
        eval(dirtPos);
    }
    cout << " done " << numChecked << " permutations " << shortesDistance << " moves " << endl;
    Utils::printMoves(pos, shortesRoute[0], N, 1);
}

void next_move(int posr, int posc, const vector<string> &board) {
    const int N = board[0].size();
    const int pos = posr * N + posc;
    if (board[posr][posc] == 'd') cout << "CLEAN" << endl;
    else next_move_permutations(pos, N, board);
}

int cleaning_bot_permute_test(int argc, char **argv)
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

    while (Niter--)
    {
        auto board = CBot::makeRandomBoard(H, W, Density);
        cout << "testing board " << ci++ << endl;
        for (auto &r : board) {
            cout << r << endl;
        }
        for (int r = 0; r < H; ++r)
            for (int c = 0; c < W; ++c)
                //if (board[r][c] != 'd')
                next_move(r, c, board);
    }
    return 0;
}

int cleaning_bot_permute_solve(int argc, char **argv)
{
    if (argc < 5){
        cout << "arguments : Width Height posx poxy board" << endl;
        return 1;
    }

    int pos[2];
    vector<string> board;
    const int W = atoi(argv[0]);
    const int H = atoi(argv[1]);
    pos[0] = atoi(argv[2]);
    pos[1] = atoi(argv[3]);
    string sboard(argv[4]);

    for (int r = 0; r < W * H; r += W) {
        board.push_back(sboard.substr(r, W));
    }
    for (auto &r : board) {
        cout << r << endl;
    }
    next_move(pos[0], pos[1], board);
    return 0;
}
}
int cleaning_bot_permute_main(int argc, char**argv)
{
    if (argc >= 1 )
    {
        if (0 == strcmp(argv[0], "test"))  return cleaning_bot_permute_test(argc-1, argv+1);
        if (0 == strcmp(argv[0], "solve")) return cleaning_bot_permute_solve(argc-1, argv+1);
    }else{
        cout << "available options are : [test|solve]" << endl;
    }
    return 1;
}
