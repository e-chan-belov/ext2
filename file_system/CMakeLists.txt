file(GLOB FS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ext2/*.c
)

add_library(file_system ${FS_SOURCES})
target_include_directories(file_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})