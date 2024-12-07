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
#define Clamp(value, min, max) ( value < min ? min : value > max ? max : value )

//////////////
/// WINDOW ///
//////////////

typedef struct {
    WINDOW* win;
    int x, y;
    int rows, cols;
} Win;

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

WINDOW* init_ncurses() {
    WINDOW* win;

    if ((win = initscr()) == NULL) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    keypad(stdscr, TRUE);
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

//////////////
/// PLAYER ///
//////////////

typedef struct {
    int x, y;
    int curr_pts, pts, max_pts;
} Player;

Player* create_player(int row, int col) {
    Player* player = malloc(sizeof(Player));
    player->y = row;
    player->x = col;
    player->curr_pts = 0;
    player->max_pts = 0;
    player->pts = 0;
    return player;
}

////////////
/// GAME ///
////////////

typedef enum {
    GameExit,
    GameMenu,
    GameSettings,
    GameSettingsEdit,
    GamePlaying,
    GamePaused,
} GameState;

typedef enum {
    Simple,
    Clean,
    Wrapped
} BorderType;

typedef struct {
    BorderType border_type;
    int width, height;
    long long seed;
} GameConfig;

typedef struct {
    int selected;
    int setting;
} MenuData;

typedef struct {
    Player* player;
    int* lines;
} GameData;

typedef union {
    MenuData menu_data; // when GameState is GameMenu or GameSettings or GameSettingsEdit
    GameData game_data; // when GameState is GamePlaying
} GameInfo;

typedef struct {
    Win* top_win;
    Win* main_win;
    Win* bottom_win;
    GameConfig config;
    GameState state;
    GameInfo context_data;
} Game;

void redraw_border(const Game* game) {
    wclear(game->bottom_win->win);
    switch (game->config.border_type) {
        case Simple:
            mvwin(game->main_win->win, 2, 0);
            mvwin(game->bottom_win->win, game->main_win->rows + 1, 0);
            wborder(game->top_win->win, '|', '|', '-', '-', '+', '+', '+', '+');
            wborder(game->main_win->win, '|', '|', '-', '-', '+', '+', '+', '+');
            wborder(game->bottom_win->win, '|', '|', '-', '-', '+', '+', '+', '+');
            break;
        case Clean:
            mvwin(game->main_win->win, 3, 0);
            mvwin(game->bottom_win->win, game->main_win->rows + 3, 0);
            box(game->top_win->win, 0, 0);
            box(game->main_win->win, 0, 0);
            box(game->bottom_win->win, 0, 0);
            break;
        case Wrapped:
            mvwin(game->main_win->win, 2, 0);
            mvwin(game->bottom_win->win, game->main_win->rows + 1, 0);
            wborder(game->top_win->win, 0, 0, '=', '=', '[', ']', '[', ']');
            wborder(game->main_win->win, 0, 0, '=', '=', '[', ']', '[', ']');
            wborder(game->bottom_win->win, 0, 0, '=', '=', '[', ']', '[', ']');
            break;
    }
    wrefresh(game->top_win->win);
    wrefresh(game->main_win->win);
    mvwprintw(game->bottom_win->win, 1, 1, "Piotr Lempkowski s203350");
    wrefresh(game->bottom_win->win);
}

Game* create_game() {
    Game* game = malloc(sizeof(Game));
    game->state = GameMenu;
    game->context_data.menu_data.selected = 0;

    GameConfig config = {Simple, 31, 21, time(NULL)};
    game->config = config;
    // game->config = malloc(sizeof(GameConfig));
    // game->config.border_type = Simple;
    // game->config.seed = time(NULL);
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
                if (strcmp(border_type, "clean") == 0) game->config.border_type = Clean;
                else if (strcmp(border_type, "wrapped") == 0) game->config.border_type = Wrapped;
                else game->config.border_type = Simple;
            } else if (strcmp(name, "size") == 0) {
                fscanf(config_file, "%d %d", &game->config.width, &game->config.height);
            } else if (strcmp(name, "seed") == 0) {
                fscanf(config_file, "%ld", &game->config.seed);
            }
        }
        fclose(config_file);
    }
    srand(game->config.seed);

    // game->player = create_player(rows - 5, cols / 2);
    // int* lines = malloc(sizeof(int) * (rows - 5));
    // for (int i = 0; i < rows - 4; i++) {
    //     lines[i] = RA(0, 2);
    // }
    // lines[rows - 6] = 0;
    // game->lines = lines;

    game->top_win = create_window(3, game->config.width, 0, 0);
    game->main_win = create_window(game->config.height, game->config.width, 2, 0);
    keypad(game->main_win->win, TRUE);
    game->bottom_win = create_window(3, game->config.width, game->config.height + 1, 0);
    mvwprintw(game->top_win->win, 1, game->main_win->cols / 2 - 3, "FROGGED");
    redraw_border(game);
    return game;
}

//////////////////
/// GAME MENUS ///
//////////////////

void print_centered_list(Game* game, char** string_list, int list_length) {
    clear_win(game->main_win);
    for (int i = 0; i < list_length; i++) {
        int offset = i == game->context_data.menu_data.selected ? -1 : 1;
        int centeredY = game->main_win->rows / 2 - list_length / 2 + i;
        int centeredX = game->main_win->cols / 2 - (int) strlen(string_list[i]) / 2 + offset;
        mvwprintw(game->main_win->win, centeredY, centeredX, i == game->context_data.menu_data.selected ? "> %s <" : "%s", string_list[i]);
    }
    wrefresh(game->main_win->win);
}

void handle_select_menu(Game* game, int key, int max, void (*accept)(Game*)) {
    switch (key) {
        case 'w':
            game->context_data.menu_data.selected -= 1;
            break;
        case 's':
            game->context_data.menu_data.selected += 1;
            break;
        case 'e':
        case ' ':
            accept(game);
            break;
        default: break;
    }
    game->context_data.menu_data.selected = Clamp(game->context_data.menu_data.selected, 0, max - 1);
}

void select_menu_cb(Game* game) {
    switch (game->context_data.menu_data.selected) {
        case 0:
            game->state = GamePlaying;
            break;
        case 1:
            game->context_data.menu_data.selected = 0;
            game->state = GameSettings;
            break;
        case 2:
            game->state = GameExit;
            break;
        default: break;
    }
}

void game_menu(Game* game) {
    mvwprintw(game->top_win->win, 1, game->main_win->cols / 2 - 3, "FROGGED");
    char* select_menu[3] = {"Start Game", "Settings", "Exit"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, select_menu, list_length);


    int key = wgetch(game->main_win->win);
    handle_select_menu(game, key, list_length, select_menu_cb);
    // switch (key) {
    //     case 'w':
    //     case KEY_UP:
    //         game->context_data.menu_data.selected -= 1;
    //         break;
    //     case 's':
    //     case KEY_DOWN:
    //         game->context_data.menu_data.selected += 1;
    //         break;
    //     case 'e':
    //     case ' ':
    //         switch (game->context_data.menu_data.selected) {
    //             case 0:
    //                 game->state = GamePlaying;
    //                 break;
    //             case 1:
    //                 game->context_data.menu_data.selected = 0;
    //                 game->state = GameSettings;
    //                 break;
    //             case 2:
    //                 game->state = GameExit;
    //                 break;
    //             default: break;
    //         }
    //         break;
    //     default: break;
    // }
    // game->context_data.menu_data.selected = Clamp(game->context_data.menu_data.selected, 0, list_length - 1);
}

void game_settings_menu(Game* game) {
    char* select_menu[4] = {"Border", "Size", "Seed", "Back"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, select_menu, list_length);

    int key = wgetch(game->main_win->win);
    switch (key) {
        case 'w':
        case KEY_UP:
            game->context_data.menu_data.selected -= 1;
            break;
        case 's':
        case KEY_DOWN:
            game->context_data.menu_data.selected += 1;
            break;
        case 'e':
        case ' ':
            switch (game->context_data.menu_data.selected) {
                // case 0:
                //     game->state = GamePlaying;
                //     break;
                // case 1:
                //     game->context_data.menu_data.selected = 0;
                //     game->state = GameSettings;
                //     break;
                case 3:
                    game->context_data.menu_data.selected = 0;
                    game->state = GameMenu;
                    break;
                default: break;
            }
            break;
        default: break;
    }
    game->context_data.menu_data.selected = Clamp(game->context_data.menu_data.selected, 0, list_length - 1);
}

void game_settings_edit(Game* game) {
    char* select_menu[4] = {"Simple", "Clean ", "Wrapped", "Return w/o saving"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, select_menu, list_length);

    int key = wgetch(game->main_win->win);
    switch (key) {
        case 'w':
            game->context_data.menu_data.selected -= 1;
            break;
        case 's':
            game->context_data.menu_data.selected += 1;
            break;
        case 'e':
        case ' ':
            switch (game->context_data.menu_data.selected) {
                case 0:
                    game->config.border_type = Simple;
                    break;
                case 1:
                    game->config.border_type = Clean;
                    break;
                case 2:
                    game->config.border_type = Wrapped;
                    break;
                default: break;
            }
            redraw_border(game);
            game->context_data.menu_data.selected = 0;
            game->state = GameSettings;
            break;
        default: break;
    }
    game->context_data.menu_data.selected = Clamp(game->context_data.menu_data.selected, 0, list_length - 1);
}

typedef enum {
    None,
    Exit,
} CharRet;

// CharRet handle_input(const Game* g) {
//     usleep(500);
//     const int ch = wgetch(g->main_win->win);
//     Player* pl = g->player;
//     switch (ch) {
//         case 'w':
//             if (pl->y > 1) {
//                 pl->y--;
//                 g->player->curr_pts++;
//             } else pl->y = 1;
//             break;
//         case 's':
//             if (pl->y < g->main_win->rows - 2) {
//                 pl->y++;
//                 g->player->curr_pts--;
//             } else pl->y = g->main_win->rows - 2;
//             break;
//         case 'a':
//             if (pl->x > 1) pl->x--;
//             else pl->x = 1;
//             break;
//         case 'd':
//             if (pl->x < g->main_win->cols - 2) pl->x++;
//             else pl->x = g->main_win->cols - 2;
//             break;
//         case 'q':
//             return Exit;
//         default:
//             return None;
//     }
//     return None;
// }

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

// void draw_frog(const Game* g) {
//     change_color(g->main_win, g->lines[g->player->y - 1]);
//     mvwprintw(g->main_win->win, g->player->y, g->player->x, "F");
// }

// void draw_game(const Game* g) {
//     for (int i = 1; i < g->main_win->rows - 1; i++) {
//         change_color(g->main_win, g->lines[i - 1]);
//         for (int j = 1; j < g->main_win->cols - 1; j++) {
//             mvwprintw(g->main_win->win, i, j, " ");
//         }
//     }
// }

// void draw_pts(const Game* g) {
//     mvwprintw(g->top_win->win, 1, 1, "PTS: %d | MAX PTS: %d", g->player->pts, g->player->max_pts);
//     wrefresh(g->top_win->win);
// }

// void calc_pts(Game* game) {
//     if (game->player->curr_pts > game->player->pts) {
//         game->player->pts = game->player->curr_pts;
//     }
//     if (game->player->pts > game->player->max_pts) {
//         game->player->max_pts = game->player->pts;
//     }
// }


int main() {
    WINDOW* main = init_ncurses();
    Game* game = create_game(main);
    while (1) {
        switch (game->state) {
            case GameMenu:
                game_menu(game);
                break;
            case GameSettings:
                game_settings_menu(game);
                break;
            case GameSettingsEdit:
                game_settings_edit(game);
                break;
            case GamePlaying:
            // break;
            case GamePaused:
                break;
            case GameExit:
                delwin(game->top_win->win);
                delwin(game->main_win->win);
                delwin(game->bottom_win->win);
                free(game->top_win);
                free(game->main_win);
                free(game->bottom_win);
                free(game);
                endwin();
                return 0;
        }
        // playing
        // CharRet code = handle_input(g);
        // calc_pts(g);
        // if (code == Exit) break;
        // clear_win(g->top_bar);
        // draw_pts(g);
        // draw_game(g);
        // draw_frog(g);
    }
}
