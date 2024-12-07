#include <limits.h>
#include "util.h"
#include "window.h"

int digit_amount(long long n) {
    int r = 1;
    if (n < 0) n = n == INT_MIN ? INT_MAX : -n;
    while (n > 9) {
        n /= 10;
        r++;
    }
    return r;
}

void change_color(Win* win, int color) {
    switch (color) {
        case 1: wcolor_set(win->win, STREET_COL, NULL);
        break;
        case 2: wcolor_set(win->win, WATER_COL, NULL);
        break;
        default: wcolor_set(win->win, GRASS_COL, NULL);
        break;
    }
}