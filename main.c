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

typedef enum {
    Simple,
    Clean,
    Extra
} BorderType;

typedef struct {
    BorderType border_type;
    int width, height;
    long long seed;
} GameConfig;

typedef struct {
    Win* top_bar;
    Win* game_win;
    Win* bottom_bar;
    GameConfig* config;
    Player* player;
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

Win* create_window(int rows, int cols, int y, int x) {
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

Player* create_player(int row, int col) {
    Player* p = malloc(sizeof(Player));
    p->y = row;
    p->x = col;
    p->curr_pts = 0;
    p->max_pts = 0;
    p->pts = 0;
    return p;
}

Game* create_game() {
    Game* game = malloc(sizeof(Game));
    game->state = GameMenu;

    game->config = malloc(sizeof(GameConfig));
    game->config->border_type = Simple;
    game->config->height = 31;
    game->config->width = 21;
    game->config->seed = time(NULL);
    FILE* config_file = fopen("config.txt", "r");
    if (config_file == NULL) {
        fclose(config_file);
        FILE* config_create = fopen("config.txt", "w");
        // filling config with default values
        fprintf(config_create, "border simple\n");
        fprintf(config_create, "size 31 21\n");
        fclose(config_create);
    } else {
        while (!feof(config_file)) {
            char name[20];
            fscanf(config_file, "%s", name);
            if (strcmp(name, "border") == 0) {
                char border_type[20];
                fscanf(config_file, "%s", border_type);
                if (strcmp(border_type, "clean") == 0) game->config->border_type = Clean;
                else if (strcmp(border_type, "extra") == 0) game->config->border_type = Extra;
                else game->config->border_type = Simple;
            } else if (strcmp(name, "size") == 0) {
                fscanf(config_file, "%d %d", &game->config->width, &game->config->height);
            } else if (strcmp(name, "seed") == 0) {
                fscanf(config_file, "%ld", &game->config->seed);
            }
        }
        fclose(config_file);
    }
    srand(game->config->seed);

    // game->player = create_player(rows - 5, cols / 2);
    // int* lines = malloc(sizeof(int) * (rows - 5));
    // for (int i = 0; i < rows - 4; i++) {
    //     lines[i] = RA(0, 2);
    // }
    // lines[rows - 6] = 0;
    // game->lines = lines;

    game->top_bar = create_window(3, game->config->width, 0, 0);
    game->game_win = create_window(game->config->height, game->config->width, 2, 0);
    game->bottom_bar = create_window(3, game->config->width, game->config->height + 1, 0);
    switch (game->config->border_type) {
        case Simple:
            wborder(game->top_bar->win, '|', '|', '-', '-', '+', '+', '+', '+');
            wborder(game->game_win->win, '|', '|', '-', '-', '+', '+', '+', '+');
            wborder(game->bottom_bar->win, '|', '|', '-', '-', '+', '+', '+', '+');
            break;
        case Clean:
            mvwin(game->game_win->win, 3, 0);
            mvwin(game->bottom_bar->win, game->config->height + 3, 0);
            box(game->top_bar->win, 0, 0);
            box(game->game_win->win, 0, 0);
            box(game->bottom_bar->win, 0, 0);
            break;
        case Extra:
            wborder(game->top_bar->win, 0, 0, '=', '=', '[', ']', '[', ']');
            wborder(game->game_win->win, 0, 0, '=', '=', '[', ']', '[', ']');
            wborder(game->bottom_bar->win, 0, 0, '=', '=', '[', ']', '[', ']');
            break;
    }
    mvwprintw(game->top_bar->win, 1, game->config->width/2-3, "FROGGED");
    wrefresh(game->top_bar->win);
    wrefresh(game->game_win->win);
    mvwprintw(game->bottom_bar->win, 1, 1, "Piotr Lempkowski s203350");
    wrefresh(game->bottom_bar->win);
    return game;
}

typedef enum {
    None,
    Exit,
} CharRet;

CharRet handle_input(const Game* g) {
    usleep(500);
    const int ch = wgetch(g->game_win->win);
    Player* pl = g->player;
    switch (ch) {
        case 'w':
            if (pl->y > 1) {
                pl->y--;
                g->player->curr_pts++;
            } else pl->y = 1;
            break;
        case 's':
            if (pl->y < g->game_win->rows - 2) {
                pl->y++;
                g->player->curr_pts--;
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
    change_color(g->game_win, g->lines[g->player->y - 1]);
    mvwprintw(g->game_win->win, g->player->y, g->player->x, "F");
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
    mvwprintw(g->top_bar->win, 1, 1, "PTS: %d | MAX PTS: %d", g->player->pts, g->player->max_pts);
    wrefresh(g->top_bar->win);
}

void calc_pts(Game* game) {
    if (game->player->curr_pts > game->player->pts) {
        game->player->pts = game->player->curr_pts;
    }
    if (game->player->pts > game->player->max_pts) {
        game->player->max_pts = game->player->pts;
    }
}

void game_menu(Game* game) {
}

int main() {
    WINDOW* main = init_ncurses();
    Game* g = create_game(main, 26, 27);
    while (1) {
        CharRet code = handle_input(g);
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
