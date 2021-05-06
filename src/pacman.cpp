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

namespace Pac
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

    string getMove(Board::FieldId_t from, Board::FieldId_t to)
    {
        int fromr, fromc, tor, toc;
        Board::id2pos(from,fromr,fromc);
        Board::id2pos(to,tor,toc);
        if (tor > fromr) return "RIGHT";
        if (tor < fromr) return "LEFT";
        if (toc > fromc) return "DOWN";
        if (toc < fromc) return "UP";
        return "STAY";
    }
    int pacman_dfs_solve(int argc, char**argv)
    {
        const auto [pacr,pacc,foodr,foodc,ROWS,COLS,desc] = loadInputFile(argv[0]);
        Board board = Board::create(desc, '-');
        //board.dump();
        auto path = board.findPath(Board::pos2id(pacr,pacc), Board::pos2id(foodr,foodc));
        return 0;
    }
}
int pacman_main(int argc, char**argv)
{
    if (argc >=1 )
    {
        if (0 == strcmp(argv[0], "dfs_test"))  return Pac::pacman_dfs_test(argc-1, argv+1);
        if (0 == strcmp(argv[0], "dfs_solve")) return Pac::pacman_dfs_solve(argc-1, argv+1);
    }else{
        cout << "available options are : [dfs_test|dfs_solve]" << endl;
    }
    return 1;
}
