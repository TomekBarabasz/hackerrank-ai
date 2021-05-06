//
// Created by barabasz on 05.05.2021.
//

#include "utils.h"
#include <iostream>
#include <string>

using namespace std;

namespace Utils
{
    void printMoves(int pos, int dest, int N, int cnt)
    {
        const string commands[] = {"LEFT", "RIGHT", "UP", "DOWN"};
        const int dx = dest%N - pos%N;
        const int dy = (dest-pos) / N;
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
}
