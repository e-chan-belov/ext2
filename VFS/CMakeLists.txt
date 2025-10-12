file(GLOB VFS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ext2/*.c
)

add_library(VFS ${VFS_SOURCES})
target_include_directories(VFS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})