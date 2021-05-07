//
// Created by barabasz on 05.05.2021.
//

#ifndef CLEANING_BOT_BOARD_H
#define CLEANING_BOT_BOARD_H

#include <string>
#include <vector>
#include <map>
#include <set>

using std::string;
using std::vector;
using std::map;
using std::set;

namespace CBot
{
    vector<string> makeRandomBoard(int h, int w, int dirt_cnt);
    vector<int> findDirt(const vector<string> &board);
    int evalRoute(int start, const vector<int> &route, int n);
    int calcSteps(int p0, int p1, int N);
}
namespace Pac
{
    struct Board
    {
        typedef unsigned int FieldId_t;
        struct Neighbours_t {
            FieldId_t ids[4]; unsigned int cnt;
            Neighbours_t() : cnt(0) {}
            Neighbours_t(std::initializer_list<FieldId_t> init);
            void add(FieldId_t f) { ids[cnt++] = f; }
        };
        typedef set<FieldId_t> Fields_t;
        typedef map<FieldId_t, Neighbours_t> Graph_t;
        Fields_t fields;
        Graph_t graph;
        unsigned int WIDTH,HEIGHT;

        static Board create(const vector<string>& description, char open='-', char blocked='%');
        void dump();
        static inline FieldId_t pos2id(int r, int c) { return r << 8 | c & 0xff;}
        static inline void id2pos(FieldId_t f, int & r, int & c) {
            c = f & 0xff;
            r = f >> 8;
        }
        static string id2s(Board::FieldId_t f);
        Neighbours_t makeNeighbours(FieldId_t f);
        vector<FieldId_t> findPath(FieldId_t from, FieldId_t to);
    };
}

#endif //CLEANING_BOT_BOARD_H
