cmake_minimum_required(VERSION 3.17)
project(ex1)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-std=c99 -Wall -Wextra -Wpedantic -Wconversion -Wunused -Wshadow -Wvla -Wmissing-braces -Wunused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

#add_executable(ex1 linkedList/mergeSort.c)
#add_executable(ex1 reverseString/reverseString.c)
#add_executable(ex1 dictionary/map.c dictionary/linkedList.h dictionary/linkedList.c systemChess/ChessSystem.c)
#add_executable(ex1 systemChess/ChessSystem.c systemChess/game.h)
add_executable(ex1 dictionary/test_utilities.h dictionary/map_example_test.c dictionary/map.c dictionary/map.h)
#add_executable(ex1 dictionary/test_utilities.h dictionary/basic_test.c dictionary/map.c dictionary/map.h)

