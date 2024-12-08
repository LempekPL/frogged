#include <limits.h>
#include "util.h"

int digit_amount(long long n) {
    int r = 1;
    if (n < 0) n = n == INT_MIN ? INT_MAX : -n;
    while (n > 9) {
        n /= 10;
        r++;
    }
    return r;
}

void change_color(const Win* win, LineType line_type) {
    switch (line_type) {
        case LineGrass:
            wcolor_set(win->win, GRASS_COL, NULL);
        break;
        case LineRoad:
            wcolor_set(win->win, ROAD_COL, NULL);
        break;
        case LineWater:
            wcolor_set(win->win, WATER_COL, NULL);
        break;
    }
}

void print_top(Game* game, char* string, int centered) {
    clear_win(game->top_win);
    if (centered) {
        mvwprintw(game->top_win->win, 1, centerX(game, string), string);
    } else {
        mvwprintw(game->top_win->win, 1, 1, string);
    }
    wrefresh(game->top_win->win);
}