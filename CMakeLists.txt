cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME ext2_test_first)
project(${PROJECT_NAME})

set(CMAKE_C_STANDARD 90)

include_directories(file_system)
include_directories(VFS)

add_subdirectory(src)

add_executable(app 
    src/main.c)

add_subdirectory(file_system)
add_subdirectory(VFS)

target_link_libraries(app PRIVATE file_system)
target_link_libraries(app PRIVATE VFS)