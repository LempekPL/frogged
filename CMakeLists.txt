cmake_minimum_required(VERSION 3.30)
project(frogged C)

set(CMAKE_C_STANDARD 11)

include_directories(G:/msys64/mingw64/include)

link_directories(G:/msys64/mingw64/lib)

add_executable(frogged main.c)

target_link_libraries(frogged ncursesw)