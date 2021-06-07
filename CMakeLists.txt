cmake_minimum_required(VERSION 3.16)
find_package(PkgConfig REQUIRED)
project(c++20)
set(CMAKE_CXX_STANDARD 20)
project(hackerrank-ai)
include_directories(${GTEST_INCLUDE_DIRS}  src/ src/lib/)
link_directories( ${GTEST_LIBRARY_DIRS} )
add_definitions(${GTEST_CFLAGS_OTHER})
pkg_check_modules(GTEST REQUIRED gtest>=1.8.1)

add_subdirectory(src/lib)
add_executable(hackerrank-ai src/main.cpp src/cleaning-bot-permute.cpp src/cleaning-bot-search.cpp src/pacman.cpp)
target_link_libraries(hackerrank-ai utils board stdc++)

add_executable(cbot-large src/cleaning-bot-large.cpp src/lib/board.cpp src/lib/utils.cpp)
target_link_libraries(cbot-large utils board stdc++)

add_subdirectory(src/MazeEscape)

add_executable(unit_tests test/test_pac_board.cpp test/test_pac.cpp src/pacman.cpp test/test_maze_escape.cpp)
target_link_libraries(unit_tests gtest gtest_main pthread board maze-escape-lib)
#add_test(unit_tests)