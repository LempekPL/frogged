#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_COL 0
#define PLAYER_COL 1
#define GROUND_COL 2
#define CAR_COL 3
#define SAFE_GROUND_COL 4
#define WATER_COL 5

#define RA(min, max) ( (min) + rand() % ((max) - (min) + 1) )

typedef struct {
    WINDOW* win;
    int x, y;
    int rows, cols;
} Win;

typedef struct {
    int x, y;
    int curr_pts, pts, max_pts;
} Player;

typedef struct {
    Win* top_bar;
    Win* game_win;
    Player* pl;
    int* lines;
} Game;

WINDOW* init_ncurses() {
    WINDOW* win;

    if ((win = initscr()) == NULL) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    start_color();
    init_pair(DEFAULT_COL, COLOR_WHITE, COLOR_BLACK);
    init_pair(PLAYER_COL, COLOR_WHITE, COLOR_GREEN);
    init_pair(GROUND_COL, COLOR_WHITE, COLOR_BLACK);
    init_pair(WATER_COL, COLOR_WHITE, COLOR_CYAN);
    init_pair(SAFE_GROUND_COL, COLOR_WHITE, COLOR_GREEN);

    noecho();
    curs_set(0);
    return win;
}

void clear_win(const Win* W) {
    for (int i = 1; i < W->rows - 1; i++)
        for (int j = 1; j < W->cols - 1; j++)
            mvwprintw(W->win, i, j, " ");
}

Win* create_window(WINDOW* parent, int rows, int cols, int y, int x) {
    Win* w = malloc(sizeof(Win));
    w->win = subwin(parent, rows, cols, y, x);
    w->rows = rows;
    w->cols = cols;
    w->y = y;
    w->x = x;
    clear_win(w);
    wrefresh(w->win);
    return w;
}

Player* create_player(int row, int col) {
    Player* p = malloc(sizeof(Player));
    p->y = row;
    p->x = col;
    p->curr_pts = 0;
    p->max_pts = 0;
    p->pts = 0;
    return p;
}

Game* create_game(WINDOW* main, int rows, int cols) {
    Game* g = malloc(sizeof(Game));
    g->pl = create_player(rows - 5, cols / 2);
    int* lines = malloc(sizeof(int) * (rows - 5));
    for (int i = 0; i < rows - 4; i++) {
        lines[i] = RA(0, 2);
    }
    lines[rows - 6] = 0;
    g->lines = lines;
    g->top_bar = create_window(main, 3, cols, 0, 0);
    wcolor_set(g->top_bar->win, GROUND_COL, NULL);
    clear_win(g->top_bar);
    wborder(g->top_bar->win, '|', '|', '-', '-', '+', '+', '+', '+');
    nodelay(g->top_bar->win, 1);
    wrefresh(g->top_bar->win);

    g->game_win = create_window(main, rows - 3, cols, 2, 0);
    wcolor_set(g->game_win->win, GROUND_COL, NULL);
    clear_win(g->game_win);
    wborder(g->game_win->win, '|', '|', '-', '-', '+', '+', '+', '+');
    nodelay(g->game_win->win, 1);
    wrefresh(g->game_win->win);
    return g;
}

void free_game(Game* g) {
    delwin(g->top_bar->win);
    delwin(g->game_win->win);
    free(g->game_win);
    free(g);
}

typedef enum {
    None,
    Exit,
} State;

State handle_input(const Game* g) {
    usleep(500);
    const int ch = wgetch(g->game_win->win);
    Player* pl = g->pl;
    switch (ch) {
        case 'w':
            if (pl->y > 1) {
                pl->y--;
                g->pl->curr_pts++;
            } else pl->y = 1;
            break;
        case 's':
            if (pl->y < g->game_win->rows - 2) {
                pl->y++;
                g->pl->curr_pts--;
            } else pl->y = g->game_win->rows - 2;
            break;
        case 'a':
            if (pl->x > 1) pl->x--;
            else pl->x = 1;
            break;
        case 'd':
            if (pl->x < g->game_win->cols - 2) pl->x++;
            else pl->x = g->game_win->cols - 2;
            break;
        case 'q':
            return Exit;
        default:
            return None;
    }
    return None;
}

void change_color(Win* win, int color) {
    switch (color) {
        case 1: wcolor_set(win->win, GROUND_COL, NULL);
        break;
        case 2: wcolor_set(win->win, WATER_COL, NULL);
        break;
        default: wcolor_set(win->win, SAFE_GROUND_COL, NULL);
        break;
    }
}

void draw_frog(const Game* g) {
    change_color(g->game_win, g->lines[g->pl->y - 1]);
    mvwprintw(g->game_win->win, g->pl->y, g->pl->x, "F");
}

void draw_game(const Game* g) {
    for (int i = 1; i < g->game_win->rows - 1; i++) {
        change_color(g->game_win, g->lines[i - 1]);
        for (int j = 1; j < g->game_win->cols - 1; j++) {
            mvwprintw(g->game_win->win, i, j, " ");
        }
    }
}

void draw_pts(const Game* g) {
    mvwprintw(g->top_bar->win, 1, 1, "PTS: %d | MAX PTS: %d", g->pl->pts, g->pl->max_pts);
    wrefresh(g->top_bar->win);
}

void calc_pts(Game* g) {
    if (g->pl->curr_pts > g->pl->pts) {
        g->pl->pts = g->pl->curr_pts;
    }
    if (g->pl->pts > g->pl->max_pts) {
        g->pl->max_pts = g->pl->pts;
    }
}

int main() {
    srand(time(NULL));
    WINDOW* main = init_ncurses();
    Game* g = create_game(main, 26, 27);
    while (1) {
        State code = handle_input(g);
        calc_pts(g);
        if (code == Exit) break;

        clear_win(g->top_bar);
        draw_pts(g);
        draw_game(g);
        draw_frog(g);
    }
    free_game(g);
    endwin();
    return 0;
}
