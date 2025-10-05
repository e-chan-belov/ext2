cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME ext2_test_first)
project(${PROJECT_NAME})

set(CMAKE_C_STANDARD 90)

add_subdirectory(library)
add_subdirectory(src)