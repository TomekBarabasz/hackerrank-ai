//
// Created by barabasz on 05.05.2021.
//

#include <iostream>
#include <cstring>
#include <fstream>
#include <tuple>
#include "board.h"
#include "utils.h"

using namespace std;
using namespace Pac;

namespace
{
    int pacman_dfs_test(int argc, char**argv)
    {
        cout << "pacman_dfs_test" <<endl;
        return 0;
    }
    std::tuple<int,int,int,int,int,int,vector<string>> loadInputFile(const char* filename)
    {
        ifstream input(filename);
        int pacr,pacc;
        int foodr,foodc;
        int ROWS,COLS;
        vector<string> board;
        input>>pacr>>pacc;
        input>>foodr>>foodc;
        input>>ROWS>>COLS;
        for(int i=0; i<ROWS; i++) {
            string s;
            input >> s;
            board.push_back(s);
        }
        return {pacr,pacc,foodr,foodc,ROWS,COLS,board};
    }
    string id2s(Board::FieldId_t f)
    {
        int r,c;
        Board::id2pos(f,r,c);
        return "(" + std::to_string(r) + "," + std::to_string(c) + ")";
    }
    void dumpBoard(const Board &board)
    {
        cout << "fields" << endl;
        for (const auto & f : board.fields) {
            cout << id2s(f) << endl;
        }
        cout << "edges" << endl;
        for(const auto & kv : board.graph) {
            cout << id2s(kv.first) << ":";
            const auto & ne = kv.second;
            for (int i=0;i<ne.cnt; ++i) cout << id2s(ne.ids[i]) << ",";
            cout << endl;
        }
    }

    int pacman_dfs_solve(int argc, char**argv)
    {
        const auto [pacr,pacc,foodr,foodc,ROWS,COLS,desc] = loadInputFile(argv[0]);
        Board board = Board::create(desc, '-');
        //dumpBoard(board);
        auto path = board.findPath(Board::pos2id(pacr,pacc), Board::pos2id(foodr,foodc));
        return 0;
    }
}
int pacman_main(int argc, char**argv)
{
    if (argc >=1 )
    {
        if (0 == strcmp(argv[0], "dfs_test"))  return pacman_dfs_test(argc-1, argv+1);
        if (0 == strcmp(argv[0], "dfs_solve")) return pacman_dfs_solve(argc-1, argv+1);
    }else{
        cout << "available options are : [dfs_test|dfs_solve]" << endl;
    }
    return 1;
}
