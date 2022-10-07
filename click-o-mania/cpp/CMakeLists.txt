cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_COMPILER /usr/lib/llvm-14/bin/clang)
find_package(PkgConfig REQUIRED)
set(CMAKE_CXX_STANDARD 20)
project(click-o-mania)
include_directories(${GTEST_INCLUDE_DIRS} src)
link_directories( ${GTEST_LIBRARY_DIRS} )
add_definitions(${GTEST_CFLAGS_OTHER})
pkg_check_modules(GTEST REQUIRED gtest>=1.8.1)
add_library(click-o-mania-lib STATIC click-o-mania.cpp)
target_compile_options(click-o-mania-lib PUBLIC 
    -O3 -gdwarf-3
    -march=x86-64-v3
)
add_executable(click-o-mania main.cpp)
target_link_libraries(click-o-mania click-o-mania-lib stdc++)
target_compile_options(click-o-mania PUBLIC 
    -O3 -gdwarf-3
    -march=x86-64-v3
)
add_executable(unit_tests test-click-o-mania.cpp)
target_compile_options(unit_tests PUBLIC 
    -fsanitize=address
    -gdwarf-3
    -Og
    -march=x86-64-v3
)
target_link_libraries(unit_tests click-o-mania-lib gtest gtest_main pthread stdc++)
target_link_options(unit_tests PUBLIC -fsanitize=address)
#add_test(unit_tests)
#add_executable(avx_tests avx-experiments.cpp)
#target_compile_options(avx_tests PUBLIC 
#    -fsanitize=address
#    -gdwarf-3
#    -march=x86-64-v3
#    -Og
#)
#target_link_libraries(avx_tests click-o-mania-lib gtest gtest_main pthread stdc++)
#target_link_options(avx_tests PUBLIC -fsanitize=address)
