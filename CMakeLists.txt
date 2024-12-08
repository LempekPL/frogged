cmake_minimum_required(VERSION 3.30)
project(frogged C)

set(CMAKE_C_STANDARD 11)

if(WIN32)
    include_directories(G:/msys64/mingw64/include)
    link_directories(G:/msys64/mingw64/lib)
else()
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
endif()

add_executable(frogged main.c
        window.c
        window.h
        game.c
        game.h
        player.c
        player.h
        util.c
        util.h
        config.c
        config.h
        menus.c
        menus.h
        game_loop.c
        game_loop.h
        car.c
        car.h
        timer.c
        timer.h
        lines.c
        lines.h
        levels.c
        levels.h)

target_link_libraries(frogged ncursesw)