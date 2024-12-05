cmake_minimum_required(VERSION 3.30)
project(frogged C)

set(CMAKE_C_STANDARD 11)

# Add the path to the ncurses header files
include_directories(G:/msys64/mingw64/include)

# Add the path to the ncurses library files
link_directories(G:/msys64/mingw64/lib)

# Add the source files
add_executable(frogged main.c)

# Link ncurses library (note: using -lncurses might not be sufficient on Windows)
target_link_libraries(frogged ncursesw)  # Try using ncursesw, which is more common