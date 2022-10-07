//
// Created by barabasz on 20.05.2021.
//

#include <gtest/gtest.h>
#include <MazeEscape/maze-escape.h>
#include <fstream>

using namespace MazeEscape;

Area makeCorner()
{
    vector<string> area = { "###",
                            "#--",
                            "#--" };
    return area;
}

Area makeLeftWall()
{
    vector<string> area = { "#--",
                            "#--",
                            "#--" };
    return area;
}

Area makeMaze1()
{
    vector<string> maze = {"#######",
                           "#--#--#",
                           "#-----#",
                           "##---##",
                           "#-----#",
                           "#--#--#",
                           "#######"};
    return maze;
}

TEST(MazeEscape, corner_0)
{
    auto area = makeCorner();
    Agent *a = Agent::create("nomem");
    ASSERT_EQ(Direction::RIGHT, a->makeMove(area));
    delete a;
}
TEST(MazeEscape, corner_90)
{
    Area area = rot90cw(makeCorner());
    Agent *a = Agent::create("nomem");
    ASSERT_EQ(Direction::LEFT, a->makeMove(area));
    delete a;
}
TEST(MazeEscape, corner_180)
{
    auto area =  rot90cw(rot90cw(makeCorner()));
    Agent *a = Agent::create("nomem");
    ASSERT_EQ(Direction::UP, a->makeMove(area));
    delete a;
}
TEST(MazeEscape, corner_left_up)
{
    vector<string> area = { "###",
                            "#--",
                            "#--" };
    Agent *a = Agent::create("nomem");
    ASSERT_EQ(1, a->makeMove(area));
    delete a;
}

TEST(MazeEscape, corner_270)
{
    auto area =  rot90cw(rot90cw(rot90cw(makeCorner())));
    Agent *a = Agent::create("nomem");
    ASSERT_EQ(0, a->makeMove(area));
    delete a;
}
TEST(MazeEscape, match_3)
{
    Area wall_left("#--#--#--");
    const string wall_front = "###------";
    ASSERT_EQ(3, wall_left.matchFeature(wall_front, true));
}
TEST(MazeEscape, wall_0)
{
    auto area = makeLeftWall();
    Agent *a = Agent::create("nomem");
    ASSERT_EQ(0, a->makeMove(area));
    delete a;
}
TEST(MazeEscape, wall_90)
{
    auto area = rot90cw(makeLeftWall());
    Agent *a = Agent::create("nomem");
    ASSERT_EQ(Direction::LEFT, a->makeMove(area));
    delete a;
}
TEST(MazeEscape, wall_180)
{
    auto area = rot90cw(rot90cw(makeLeftWall()));
    Agent *a = Agent::create("nomem");
    ASSERT_EQ(Direction::UP, a->makeMove(area));
    delete a;
}
TEST(MazeEscape, wall_270)
{
    auto area = rot90cw(rot90cw(rot90cw(makeLeftWall())));
    Agent *a = Agent::create("nomem");
    ASSERT_EQ(3, a->makeMove(area));
    delete a;
}

TEST(MazeEscape, explore_init)
{
    Area ea;
    Area view({"---","---","---"});
    ea.append(view,0,0);
    ASSERT_EQ(ea.toprow(),-1);
    ASSERT_EQ(ea.leftcol(),-1);
    ASSERT_EQ(ea.rows(), 3);
    ASSERT_EQ(ea.cols(), 3);

    ASSERT_EQ(ea.getRow(0),"---");
    ASSERT_EQ(ea.getRow(1),"---");
    ASSERT_EQ(ea.getRow(2),"---");
}

TEST(MazeEscape, explore_up)
{
    Area ea;
    Area v1({"---","---","---"});
    Area v3({"###","---","---"});

    ea.append(v1, 0,0);
    ea.append(v1,-1,0);
    ea.append(v3,-2,0);

    ASSERT_EQ(ea.toprow(),-3);
    ASSERT_EQ(ea.leftcol(),-1);
    ASSERT_EQ(ea.rows(), 5);
    ASSERT_EQ(ea.cols(), 3);

    ASSERT_EQ(ea.getRow(0),"###");
    ASSERT_EQ(ea.getRow(1),"---");
    ASSERT_EQ(ea.getRow(2),"---");
    ASSERT_EQ(ea.getRow(3),"---");
    ASSERT_EQ(ea.getRow(4),"---");
}
TEST(MazeEscape, explore_down)
{
    Area ea;
    Area v1({"---","---","---"});

    ea.append(v1,0,0);
    ea.append(v1,1,0);
    ea.append(Area({"---","---","###"}),2,0);

    ASSERT_EQ(ea.toprow(),-1);
    ASSERT_EQ(ea.leftcol(),-1);
    ASSERT_EQ(ea.rows(), 5);
    ASSERT_EQ(ea.cols(), 3);

    ASSERT_EQ(ea.getRow(0),"---");
    ASSERT_EQ(ea.getRow(1),"---");
    ASSERT_EQ(ea.getRow(2),"---");
    ASSERT_EQ(ea.getRow(3),"---");
    ASSERT_EQ(ea.getRow(4),"###");
}
TEST(MazeEscape, explore_left)
{
    Area ea;
    Area v1({"---","---","---"});

    ea.append(v1,0,0);
    ea.append(v1,0,-1);
    ea.append(Area({"#--","#--","#--"}),0,-2);

    ASSERT_EQ(ea.toprow(),-1);
    ASSERT_EQ(ea.leftcol(),-3);
    ASSERT_EQ(ea.rows(), 3);
    ASSERT_EQ(ea.cols(), 5);

    ASSERT_EQ(ea.getRow(0),"#----");
    ASSERT_EQ(ea.getRow(1),"#----");
    ASSERT_EQ(ea.getRow(2),"#----");
}
TEST(MazeEscape, explore_right)
{
    Area ea;
    Area v1({"---","---","---"});

    ea.append(v1,0,0);
    ea.append(v1,0,1);
    ea.append(Area({"--#","--#","--#"}),0,2);

    ASSERT_EQ(ea.toprow(),-1);
    ASSERT_EQ(ea.leftcol(),-1);
    ASSERT_EQ(ea.rows(), 3);
    ASSERT_EQ(ea.cols(), 5);

    ASSERT_EQ(ea.getRow(0),"----#");
    ASSERT_EQ(ea.getRow(1),"----#");
    ASSERT_EQ(ea.getRow(2),"----#");
}
TEST(MazeEscape, explore_up_left)
{
    Area ea;
    ea.append(Area({"#--","---","---"}), 0,0);
    ea.append(Area({"###","#--","---"}),-1,0);
    ea.append(Area({"###","--#","--#"}),-1,1);

    ASSERT_EQ(ea.toprow(),-2);
    ASSERT_EQ(ea.leftcol(),-1);
    ASSERT_EQ(ea.rows(), 4);
    ASSERT_EQ(ea.cols(), 4);

    ASSERT_EQ(ea.getRow(0),"####");
    ASSERT_EQ(ea.getRow(1),"#--#");
    ASSERT_EQ(ea.getRow(2),"---#");
    ASSERT_EQ(ea.getRow(3),"---x");
}
TEST(MazeEscape, explore_down_right)
{
    Area ea;
    ea.append(Area({"---","---","#--"}), 0,0);
    ea.append(Area({"---","#--","###"}), 1,0);
    ea.append(Area({"--#","--#","###"}), 1,1);


    ASSERT_EQ(ea.toprow(),-1);
    ASSERT_EQ(ea.leftcol(),-1);
    ASSERT_EQ(ea.rows(), 4);
    ASSERT_EQ(ea.cols(), 4);

    ASSERT_EQ(ea.getRow(0),"---x");
    ASSERT_EQ(ea.getRow(1),"---#");
    ASSERT_EQ(ea.getRow(2),"#--#");
    ASSERT_EQ(ea.getRow(3),"####");
}

TEST(MazeEscape, explore_mark_visited)
{
    Area ea;
    Area v1({"---","---","---"});
    Area v2({"--#","--#","--#"});

    ea.append(v1,0,0,'.');
    ea.append(v1,0,1,'.');
    ea.append(v2,0,2,'.');

    ASSERT_EQ(ea.toprow(),-1);
    ASSERT_EQ(ea.leftcol(),-1);
    ASSERT_EQ(ea.rows(), 3);
    ASSERT_EQ(ea.cols(), 5);

    ASSERT_EQ(ea.getRow(0),"----#");
    ASSERT_EQ(ea.getRow(1),"-...#");
    ASSERT_EQ(ea.getRow(2),"----#");
}
TEST(MazeEscape, getView_1)
{
    Area maze = makeMaze1();
    auto view = maze.getView(4, 2, Direction::RIGHT);
    ASSERT_EQ("--#"\
              "---"\
              "#--", view.concat());
}
TEST(MazeEscape, getView_2)
{
    Area maze = makeMaze1();
    auto view = maze.getView(2, 1, Direction::LEFT);
    ASSERT_EQ("###"\
              "#--"\
              "---", view.concat());
}
TEST(MazeEscape, moveByFeature_1)
{
    Agent *a = Agent::create("mem");
    Area maze = makeMaze1();
    int posr=5, posc=1, dir=0;

    Direction moves[] = { Direction::UP, Direction::RIGHT, Direction::LEFT, Direction::UP, Direction::LEFT, Direction::RIGHT, Direction::RIGHT, Direction::RIGHT};
    for (auto exectedMove : moves)
    {
        auto move = a->makeMove(maze.getView(posr,posc,dir));
        ASSERT_EQ(exectedMove, move);
        dir = (dir+move) % 4;
        Agent::updatePos(posr,posc,dir);
    }
    delete a;
}

TEST(MazeEScape, moveByFeature_2)
{
    Area area("###"\
              "#--"\
              "---");
    auto m = Agent::tryMoveByFeature(area);
    ASSERT_EQ(Direction::RIGHT, m);
}
TEST(MazeEScape, moveByFeature_3)
{
    Area area(  "##-"\
                "#--"\
                "#--");
    auto m = Agent::tryMoveByFeature(area);
    ASSERT_EQ(Direction::RIGHT, m);
}
TEST(MazeEScape, save_load)
{
    Area m=makeMaze1();
    std::ofstream out("tmp.txt");
    m.store(out);
    out.close();
    std::ifstream in("tmp.txt");
    Area m2(in);
    ASSERT_EQ(m.toprow(), m2.toprow());
    ASSERT_EQ(m.leftcol(), m2.leftcol());
    ASSERT_EQ(m.rows(), m2.rows());
    ASSERT_EQ(m.cols(), m2.cols());
    for (int r=0;r<m.rows();++r){
        ASSERT_EQ(m.getRow(r), m2.getRow(r));
    }
}
TEST(MazeEScape, findPattern_0)
{
    Area maze("-#--#-###");
    Area view("-#--#-###");
    auto loc = maze.findPattern(view);
    ASSERT_EQ(1, loc.size());
    ASSERT_EQ(0, loc[0].posr);
    ASSERT_EQ(0, loc[0].posc);
    ASSERT_EQ(Direction::UP, loc[0].dir);
}

TEST(MazeEScape, findPattern_90)
{
    Area maze("-#--#-###");
    Area view("-#--#-###");
    auto loc = maze.findPattern( rot90cw(view) ) ;
    ASSERT_EQ(1, loc.size());
    ASSERT_EQ(0, loc[0].posr);
    ASSERT_EQ(0, loc[0].posc);
    ASSERT_EQ(Direction::LEFT, loc[0].dir);
}
TEST(MazeEScape, findPattern_neg90)
{
    Area maze("-#--#-###");
    Area view("-#--#-###");
    auto loc = maze.findPattern( rot90ccw(view) ) ;
    ASSERT_EQ(1, loc.size());
    ASSERT_EQ(0, loc[0].posr);
    ASSERT_EQ(0, loc[0].posc);
    ASSERT_EQ(Direction::RIGHT, loc[0].dir);
}
TEST(MazeEScape, findPattern_180)
{
    Area maze("-#--#-###");
    Area view("-#--#-###");
    auto loc = maze.findPattern( rot90cw(rot90cw(view)) ) ;
    ASSERT_EQ(1, loc.size());
    ASSERT_EQ(0, loc[0].posr);
    ASSERT_EQ(0, loc[0].posc);
    ASSERT_EQ(Direction::DOWN, loc[0].dir);
}

TEST(MazeEScape, findPattern_270)
{
    Area maze("-#--#-###");
    Area view("-#--#-###");
    auto loc = maze.findPattern( rot90cw(rot90cw(rot90cw(view))) ) ;
    ASSERT_EQ(1, loc.size());
    ASSERT_EQ(0, loc[0].posr);
    ASSERT_EQ(0, loc[0].posc);
    ASSERT_EQ(Direction::RIGHT, loc[0].dir);
}

TEST(MazeEScape, findPattern_1)
{
    Area maze = makeMaze1();
    Area view("----#-###");
    auto loc = maze.findPattern(view);
    ASSERT_EQ(4, loc.size());
    ASSERT_EQ(Direction::DOWN, loc[0].dir);
    ASSERT_EQ(0, loc[0].posr);
    ASSERT_EQ(2, loc[0].posc);

    ASSERT_EQ(2, loc[1].posr);
    ASSERT_EQ(0, loc[1].posc);
    ASSERT_EQ(Direction::RIGHT, loc[1].dir);

    ASSERT_EQ(2, loc[2].posr);
    ASSERT_EQ(4, loc[2].posc);
    ASSERT_EQ(Direction::LEFT, loc[2].dir);

    ASSERT_EQ(4, loc[3].posr);
    ASSERT_EQ(2, loc[3].posc);
    ASSERT_EQ(Direction::UP, loc[3].dir);
}
TEST(MazeEScape, findClosest_1)
{
    Area maze = makeMaze1();
    vector<tuple<int,int>> pts = {{2,3},{3,4},{4,3},{3,2}};
    for (const auto & p : pts) maze.set(get<0>(p),get<1>(p),'.');
    auto loc = maze.findClosest(3, 3, '.');
    ASSERT_EQ(4, loc.size());

    ASSERT_EQ(2, loc[0].posr);
    ASSERT_EQ(3, loc[0].posc);
    ASSERT_EQ(Direction::UP, loc[0].dir);

    ASSERT_EQ(4, loc[1].posr);
    ASSERT_EQ(3, loc[1].posc);
    ASSERT_EQ(Direction::DOWN, loc[1].dir);

    ASSERT_EQ(3, loc[2].posr);
    ASSERT_EQ(2, loc[2].posc);
    ASSERT_EQ(Direction::LEFT, loc[2].dir);

    ASSERT_EQ(3, loc[3].posr);
    ASSERT_EQ(4, loc[3].posc);
    ASSERT_EQ(Direction::RIGHT, loc[3].dir);
}
TEST(MazeEScape, findClosest_2)
{
    Area maze = makeMaze1();
    vector<tuple<int,int>> pts = {{2,3},{3,5}};
    for (const auto & p : pts) maze.set(get<0>(p),get<1>(p),'.');
    auto loc = maze.findClosest(3, 3, '.');
    ASSERT_EQ(1, loc.size());

    ASSERT_EQ(2, loc[0].posr);
    ASSERT_EQ(3, loc[0].posc);
    ASSERT_EQ(Direction::UP, loc[0].dir);
}
TEST(MazeEScape, findClosest_clamp_upleft)
{
    Area maze = makeMaze1();
    maze.set(3,3,'.');
    auto loc = maze.findClosest(1, 1, '.');
    ASSERT_EQ(1, loc.size());

    ASSERT_EQ(3, loc[0].posr);
    ASSERT_EQ(3, loc[0].posc);
    ASSERT_EQ(Direction::DOWN, loc[0].dir);
}
TEST(MazeEScape, findClosest_clamp_downright)
{
    Area maze = makeMaze1();
    maze.set(3,3,'.');
    auto loc = maze.findClosest(5, 5, '.');
    ASSERT_EQ(1, loc.size());

    ASSERT_EQ(3, loc[0].posr);
    ASSERT_EQ(3, loc[0].posc);
    ASSERT_EQ(Direction::UP, loc[0].dir);
}