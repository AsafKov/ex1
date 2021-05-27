cmake_minimum_required(VERSION 3.17)
project(ex1)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-std=c99 -Wall -Wextra -Wpedantic -Wconversion -Wunused -Wshadow -Wvla -Wmissing-braces -Wunused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

#add_executable(ex1 linkedList/mergeSort.c)
#add_executable(ex1 reverseString/reverseString.c)
#add_executable(ex1 map/tests/test_utilities.h map/tests/map_tests.c map/map.c map/headers/map.h map/tests/map_tests2.c)
add_executable(ex1 systemChess/main.c systemChess/tests/chessSystemTestsExample.c systemChess/headers/chessSystem.h
        map/map.c map/node.c map/headers/map.h map/headers/node.h systemChess/headers/chessTournament.h
        systemChess/headers/chessGame.h systemChess/headers/player.h systemChess/chessTournament.c
        systemChess/chessGame.c systemChess/player.c)