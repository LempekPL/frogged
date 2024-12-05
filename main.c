#include <stdlib.h>
#include <ncursesw/ncurses.h>

#define DELAY_OFF 1

#define PLAYER_COL 1
#define GROUND_COL 2
#define CAR_COL 3

#define RA(min, max, seed) ( (min) + srand(seed) % ((max) - (min) + 1) )

typedef struct {
    WINDOW* win;
    int x, y;
    int rows, cols;
} Win;

typedef struct {
    int x, y;
} Player;

typedef struct {
    Win* win;
    Player p;
} Game;

WINDOW* init_ncurses() {
    WINDOW* win;

    if ((win = initscr()) == NULL) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    start_color();
    init_pair(PLAYER_COL, COLOR_WHITE, COLOR_GREEN);
    init_pair(GROUND_COL, COLOR_BLUE, COLOR_RED);
    init_pair(CAR_COL, COLOR_WHITE, COLOR_BLUE);

    noecho();
    curs_set(0);
    return win;
}

void clear_win(const Win* W) {
    for(int i = 0; i < W->rows; i++)
        for(int j = 0; j < W->cols; j++)
            mvwprintw(W->win, i, j," ");
}

Win* create_window(WINDOW* parent, int rows, int cols, int y, int x) {
    Win* w = malloc(sizeof(Win));
    w->win = subwin(parent, rows, cols, y, x);
    clear_win(w);
    wrefresh(w->win);
    return w;
}

void free_game(Game* g) {
    delwin(g->win->win);
    free(g->win);
    free(g);
}

int main() {
    WINDOW* main = init_ncurses();
    Game* g = malloc(sizeof(Game));
    g->win = create_window(main, 10, 10, 0, 0);

    int ch = '\0';
    while (ch != 'q') {
        ch = getch();
    }

    free_game(g);
    endwin();
    return 0;
}

