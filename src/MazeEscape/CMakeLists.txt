add_library(maze-escape-lib STATIC maze.cpp agent.cpp)
target_compile_options(maze-escape-lib PRIVATE
    -g
    ${COMPILE_OPTIONS}
)
add_executable(maze-escape maze-escape-main.cpp maze-escape.h)
target_link_libraries(maze-escape utils board stdc++ maze-escape-lib)
target_compile_options(maze-escape PRIVATE
    -g
    ${COMPILE_OPTIONS}
)
