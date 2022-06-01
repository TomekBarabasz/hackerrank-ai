//#include <iostream>
#include <string>
#include <cstring>
#include "click-o-mania.h"

using namespace std;

namespace Click_o_mania
{
SearchState makeGrid(std::initializer_list<std::string> init)
{
    static_assert(std::is_same<std::string::value_type,char>::value);
    const int nrow = init.size();
    const int ncol = init.begin()->size();
    SearchState ss(nrow,ncol);
    auto *grid = ss.grid;
    int8_t tmp[16];
    for( auto &row:init ) {
        memcpy(grid,row.c_str(),row.size());
        grid += row.size();
    }
    return ss;
}
inline void fillGroupFromPosition(const SearchState& ss, int r0, int c0, int8_t color,int8_t group, Workspace& wrk)
{
    static constexpr std::tuple<int8_t,int8_t> Directions[4] = {{-1,0},{0,-1},{0,1},{1,0}};
    auto & points = wrk.ring;
    points.push( {r0,c0} );
    while (!points.empty())
    {
        auto [r,c] = points.popfirst();
        wrk.group_map[r*ss.ncol+c] = group;
        #pragma GCC unroll 4
        for (auto [dr,dc]:Directions)
        {
            const int8_t r1=r+dr;
            if (r1 < 0 || r1 >= ss.nrow) continue;
            const int8_t c1=c+dc;
            if (c1 < 0 || c1 >= ss.ncol) continue;
            const auto idx = r1*ss.ncol+c1;
            if (color == ss.grid[idx] && 0==wrk.group_map[idx])
            {
                points.push( {r1,c1} );
            }
        }
    }
}
int partitionGrid(SearchState& ss,Workspace& wrk)
{
    const int nrow = ss.nrow;
    const int ncol = ss.ncol;
    int8_t nextGroup = 1;
    memset(wrk.group_map,0,nrow*ncol);
    int8_t *group_map = wrk.group_map;
    int8_t *grid = ss.grid;
    for (int r=0;r<nrow;++r)
        for(int c=0;c<ncol;++c)
        {
            if (0==*group_map && EmptySpace != *grid)
            {
                fillGroupFromPosition(ss,r,c,*grid,nextGroup++,wrk);
            }
            ++group_map;
            ++grid;
        }
    return nextGroup-1;
}
int collectGroups(int nrow,int ncol,const int8_t*group_map, BlockList<int8_t>& blocks)
{
    blocks.ngroups = 0;
    blocks.npoints = 0;
    return 0;
}
}