add_library(maze-escape-lib STATIC maze.cpp agent.cpp)
add_executable(maze-escape maze-escape-main.cpp maze-escape.h)
target_link_libraries(maze-escape utils board stdc++ maze-escape-lib)
