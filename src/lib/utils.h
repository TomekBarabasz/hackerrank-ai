//
// Created by barabasz on 05.05.2021.
//

#ifndef CLEANING_BOT_UTILS_H
#define CLEANING_BOT_UTILS_H

#include <vector>
#include <string>
using std::vector;
using std::string;

namespace Utils
{
    void printMoves(int pos, int dest, int N, int cnt);
    void printBoard(const vector<string>& board);
};


#endif //CLEANING_BOT_UTILS_H
