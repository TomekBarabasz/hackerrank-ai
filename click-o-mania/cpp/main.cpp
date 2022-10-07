#include <iostream>
#include "click-o-mania.h"
#include <string>
#include <vector>
#include <tuple>

using namespace Click_o_mania;
using std::vector;
using std::string;

vector<string> grid1={  "BRBBBBRRBB",
                            "RBRBBRRRRR",
                            "BRBBRRRBBB",
                            "BRRRBBBRBR",
                            "BBBBRBRRBR",
                            "BRBRRBRRRB",
                            "BBRBRBRBBB",
                            "RBRBRBRRBR",
                            "BRRRRRBBRB",
                            "RRRRBRRRRB",
                            "BRRRBBBRBR",
                            "RRBRRBBBRB",
                            "RBBBBBBBBB",
                            "BRBBRBRBBR",
                            "RBRBBBRRRB",
                            "RBRBRBRBBB",
                            "BRRRBRBRRR",
                            "RRBRBBBBBR",
                            "RBBRBBRBRB",
                            "BBBBRRBBRB"};
vector<string> grid2 =     {"BR----RRBB",
                            "RB---RRRRR",
                            "BRR-RRRBBB",
                            "BRRRBBBRBR",
                            "BBBBRBRRBR",
                            "BRBRRBRRRB",
                            "BBRBRBRBBB",
                            "RBRBRBRRBR",
                            "BRRRRRBBRB",
                            "RRRRBRRRRB",
                            "BRRRBBBRBR",
                            "RRBRRBBBRB",
                            "RBBBBBBBBB",
                            "BRBBRBRBBR",
                            "RBRBBBRRRB",
                            "RBRBRBRBBB",
                            "BRRRBRBRRR",
                            "RRBRBBBBBR",
                            "RBBRBBRBRB",
                            "BBBBRRBBRB"};
vector<string> grid3 =     {"BGOBVBGROB",
                            "YBYBVGRRGG",
                            "VYOOYYYBBB",
                            "VRRRVOBGBG",
                            "OOOVYBYYVR",
                            "VGBYYOYYGB",
                            "OOYOYVGOBV",
                            "ROROYOYYOY",
                            "BRRYYROVYB",
                            "RRYRBGGRRV",
                            "OGYGVOVYOR",
                            "GYBYYBOVGV",
                            "GOOBOVOVOB",
                            "OGVORVGVVY",
                            "GOYVOVRRGV",
                            "GORVYBYOBV",
                            "VGYYBYBGYG",
                            "RGVYOOVBOG",
                            "GVVGVVRBYB",
                            "VBOBYYOORO"};

vector<string> grid4 = {"OBYRORBYGB",
                        "YYRGOBRBYB",
                        "BOYGYRYOYR",
                        "GYYOGYOBBY",
                        "GOBGGYOGRR",
                        "OBBRBOYRBB",
                        "RRGYBRBGOY",
                        "GRYRGYGGOR",
                        "YOBOOGOBGG",
                        "YRBOGYBBGG",
                        "RRGOYBYYYY",
                        "YBBRBBRGGG",
                        "RGBYYBBRGY",
                        "YBYOBRBOGG",
                        "OBYGOGROOR",
                        "RGBOORBBBR",
                        "GOGOBRORGG",
                        "GGYBOBYRGB",
                        "YBYORYGBOR",
                        "GYROOOOBOG"};

int collect_stats(int argc, char ** argv)
{
    auto *pss = makeGrid(grid3);
    Workspace wrk(pss->nrow,pss->ncol);
    int ng = partitionGrid(*pss,wrk);
    collectGroups(*pss,ng,wrk,pss->blocks);
    auto & ss = *pss;
        
    printf("groups : %d\n", ng);
    for (int i=0;i<ng;++i)
    {
        auto nelem = std::get<2>(ss.blocks.groups[i]);
        printf("group %d nelem %d\n",i+1,nelem);
    }
    delete pss;
    return 0;
}
int main(int argc, char**argv)
{
    if (argc > 1)
    {
        if (strcmp(argv[1],"stats")==0)
        {
            return collect_stats(--argc, argv + 1);
        }
        else {
            printf("unknown command\n");
        }
    }    
    vector<string>* grids[] = {&grid1,&grid2};
    int idx=0;
    if (argc > 1){
        idx = atoi(argv[1]);
    }
    //const auto res = solve(*grids[idx]);
    const auto res = solve(grid3);

    printf("solution: %d,%d\n",res.r,res.c);
    return 0;
}
