#ifndef WINDOW_H
#define WINDOW_H

#define DEFAULT_COL 0
#define DEFAULT_SWP_COL 1
#define PLAYER_COL 2
#define STREET_COL 3
#define CAR_COL 4
#define GRASS_COL 5
#define WATER_COL 6
#define GRASS_BLACK_COL 7

#if defined(_WIN32) || defined(_WIN64) // Windows
#include <ncursesw/ncurses.h>
#else // Linux and other platforms
#include <ncurses.h>
#endif

typedef struct {
    WINDOW* win;
    int x, y;
    int rows, cols;
} Win;

WINDOW* init_ncurses();
Win* create_window(int, int, int, int);
void clear_win(const Win*);

#endif //WINDOW_H
