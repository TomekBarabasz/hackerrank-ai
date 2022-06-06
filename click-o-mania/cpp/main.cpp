#include <iostream>
#include "click-o-mania.h"
#include <string>
#include <vector>

using namespace Click_o_mania;
using std::vector;
using std::string;

int main(int argc, char**argv)
{
    vector<string> grid1={"BRBBBBRRBB",
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

vector<string> grid2 = {"BR----RRBB",
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
    vector<string>* grids[] = {&grid1,&grid2};
    int idx=0;
    if (argc > 1){
        idx = atoi(argv[1]);
    }
    //const auto res = solve(*grids[idx]);
    const auto res = solve({"BGOBVBGROB",
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
                            "VBOBYYOORO"});
    /*
    "OBYRORBYGB",
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
"GYROOOOBOG"
    */
    printf("solution: %d,%d\n",get<0>(res),get<1>(res));
    return 0;
}
