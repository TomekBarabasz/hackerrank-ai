//
// Created by barabasz on 06.05.2021.
//

#ifndef HACKERRANK_AI_PACMAN_H
#define HACKERRANK_AI_PACMAN_H

#include <lib/board.h>
#include <string>
using std::string;

namespace Pac
{
    string getMove(Board::FieldId_t from, Board::FieldId_t to);
}
#endif //HACKERRANK_AI_PACMAN_H
