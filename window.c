#include "window.h"
#include <stdlib.h>

WINDOW* init_ncurses() {
    WINDOW* win;

    if ((win = initscr()) == NULL) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    keypad(stdscr, TRUE);
    start_color();
    init_pair(DEFAULT_COL, COLOR_WHITE, COLOR_BLACK);
    init_pair(DEFAULT_SWP_COL, COLOR_BLACK, COLOR_WHITE);
    init_pair(PLAYER_COL, COLOR_WHITE, COLOR_GREEN);
    init_pair(STREET_COL, COLOR_WHITE, COLOR_BLACK);
    init_pair(WATER_COL, COLOR_WHITE, COLOR_CYAN);
    init_pair(GRASS_COL, COLOR_WHITE, COLOR_GREEN);
    init_pair(GRASS_BLACK_COL, COLOR_BLACK, COLOR_GREEN);

    noecho();
    curs_set(0);
    return win;
}

Win* create_window(const int rows, const int cols, const int y, const int x) {
    Win* window = malloc(sizeof(Win));
    window->win = newwin(rows, cols, y, x);
    window->rows = rows;
    window->cols = cols;
    window->y = y;
    window->x = x;
    clear_win(window);
    wcolor_set(window->win, DEFAULT_COL, NULL);
    nodelay(window->win, 1);
    wrefresh(window->win);
    return window;
}

void clear_win(const Win* window) {
    for (int i = 1; i < window->rows - 1; i++)
        for (int j = 1; j < window->cols - 1; j++)
            mvwprintw(window->win, i, j, " ");
}
