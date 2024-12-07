#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_COL 0
#define DEFAULT_SWP_COL 1
#define PLAYER_COL 2
#define STREET_COL 3
#define CAR_COL 4
#define GRASS_COL 5
#define WATER_COL 6
#define GRASS_BLACK_COL 7

#define RA(min, max) ( (min) + rand() % ((max) - (min) + 1) )
#define Clamp(value, min, max) ( value < min ? min : value > max ? max : value )
#define centerX(game, string) (game->main_win->cols / 2 - (int) strlen(string) / 2)

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
    Simple,
    Clean,
    Wrapped
} BorderType;

typedef struct {
    BorderType border_type;
    int width, height;
    long long seed;
    char frog;
} GameConfig;

typedef enum {
    GameExit,
    GameMenu,
    GameHelp,
    GameSettings,
    GameSettingsEdit,
    GamePlaying,
} GameState;

typedef struct {
    int selected;
    int setting;
    char text_select[10];
} MenuData;

typedef enum {
    PlayingInit,
    Playing,
    PlayingKilled,
    PlayingSuccess,
} PlayingState;

typedef struct {
    int x, y;
} Goal;

typedef struct {
    int level;
    PlayingState state;
    Player* player;
    Goal goal;
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
    wrefresh(game->bottom_win->win);
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

void save_config(const Game* game) {
    FILE* config_save = fopen("config.txt", "w");
    fprintf(config_save, "border ");
    switch (game->config.border_type) {
        case Simple:
            fprintf(config_save, "simple\n");
            break;
        case Clean:
            fprintf(config_save, "clean\n");
            break;
        case Wrapped:
            fprintf(config_save, "wrapped\n");
            break;
    }
    fprintf(config_save, "size %d %d\n", game->config.width, game->config.height);
    fprintf(config_save, "seed %lld", game->config.seed);
    fprintf(config_save, "frog %c", game->config.frog);
    fclose(config_save);
}

Game* create_game() {
    Game* game = malloc(sizeof(Game));
    game->state = GameMenu;
    game->context_data.menu_data.selected = 0;

    GameConfig config = {Simple, 31, 21, time(NULL), 'F'};
    game->config = config;
    FILE* config_file = fopen("config.txt", "r");
    if (config_file == NULL) {
        fclose(config_file);
        save_config(game);
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
                int tmpW, tmpH;
                fscanf(config_file, "%d %d", &tmpW, &tmpH);
                game->config.width = Clamp(tmpW, 31, 1000);
                game->config.height = Clamp(tmpH, 21, 1000);
            } else if (strcmp(name, "seed") == 0) {
                fscanf(config_file, "%ld", &game->config.seed);
            } else if (strcmp(name, "frog") == 0) {
                char tmpFrog;
                fscanf(config_file, "%c", &tmpFrog);
                game->config.frog = tmpFrog;
            }
        }
        fclose(config_file);
    }
    srand(game->config.seed);
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
    for (int i = 0; i < list_length; i++) {
        int offset = i == game->context_data.menu_data.selected ? -2 : 0;
        int centeredY = game->main_win->rows / 2 - list_length / 2 + i;
        mvwprintw(game->main_win->win, centeredY, centerX(game, string_list[i]) + offset,
                  i == game->context_data.menu_data.selected ? "> %s <" : "%s", string_list[i]);
    }
    wrefresh(game->main_win->win);
}

int handle_select_menu(Game* game, int key, int max) {
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
            return 1;
        default: break;
    }
    game->context_data.menu_data.selected = Clamp(game->context_data.menu_data.selected, 0, max - 1);
    return 0;
}

void game_menu(Game* game) {
    clear_win(game->top_win);
    mvwprintw(game->top_win->win, 1, game->main_win->cols / 2 - 3, "FROGGED");
    wrefresh(game->top_win->win);
    char* select_menu[5] = {"Start Game", "Levels", "How to", "Settings", "Exit"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, select_menu, list_length);

    int key = wgetch(game->main_win->win);
    if (handle_select_menu(game, key, list_length)) {
        switch (game->context_data.menu_data.selected) {
            case 0:
                game->state = GamePlaying;
                game->context_data.game_data.level = 1;
                game->context_data.game_data.state = PlayingInit;
                break;
            case 2:
                game->context_data.menu_data.selected = 0;
                game->state = GameHelp;
                break;
            case 3:
                game->context_data.menu_data.selected = 0;
                game->state = GameSettings;
                break;
            case 4:
                game->state = GameExit;
                break;
            default: break;
        }
    }
}

void game_settings_menu(Game* game) {
    char* select_menu[5] = {"Border", "Size", "Seed", "Frog", "Back"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, select_menu, list_length);

    int key = wgetch(game->main_win->win);
    if (handle_select_menu(game, key, list_length)) {
        switch (game->context_data.menu_data.selected) {
            case 0:
            case 1:
            case 2:
                game->context_data.menu_data.setting = game->context_data.menu_data.selected;
                game->state = GameSettingsEdit;
                break;
            case 3:
                game->context_data.menu_data.selected = 3;
                game->state = GameMenu;
                break;
            default: break;
        }
    }
}

int digit_amount(long long n) {
    int r = 1;
    if (n < 0) n = n == INT_MIN ? INT_MAX : -n;
    while (n > 9) {
        n /= 10;
        r++;
    }
    return r;
}

void game_settings_edit(Game* game) {
    int list_length = 1;
    char** select_menu = malloc(sizeof(char*));
    if (game->context_data.menu_data.setting == 0) {
        free(select_menu);
        list_length = 4;
        select_menu = malloc(sizeof(char*) * list_length);
        select_menu[0] = "Simple";
        select_menu[1] = "Clean ";
        select_menu[2] = "Wrapped";
        select_menu[3] = "Return without saving";
        char* message = "Selecting will save to file";
        mvwprintw(game->main_win->win, 1, game->main_win->cols / 2 - (int) strlen(message) / 2, message);
        char* val = "Current: Simple";
        switch (game->config.border_type) {
            case Clean: val = "Current: Clean";
                break;
            case Wrapped: val = "Current: Wrapped";
                break;
            default: break;
        }
        mvwprintw(game->main_win->win, 5, game->main_win->cols / 2 - (int) strlen(val) / 2, val);
        print_centered_list(game, select_menu, list_length);
    } else if (game->context_data.menu_data.setting == 2) {
        char* message = "Selecting won't save to file";
        mvwprintw(game->main_win->win, 1, centerX(game, message), message);
        char* message3 = "Leaving 0 will";
        mvwprintw(game->main_win->win, 4, centerX(game, message3), message3);
        char* message4 = "use time(NULL) as seed";
        mvwprintw(game->main_win->win, 5, centerX(game, message4), message4);
        char* message2 = "Click TAB to accept";
        mvwprintw(game->main_win->win, 7, centerX(game, message2), message2);
        mvwprintw(game->main_win->win, 10, game->main_win->cols / 2 - 10, "> %lld", game->config.seed);
        mvwprintw(game->main_win->win, 10, game->main_win->cols / 2 + 10, "<");
        wmove(game->main_win->win, 10, game->main_win->cols / 2 + digit_amount(game->config.seed) - 8);
        curs_set(1);
    } else {
        free(select_menu);
        select_menu = malloc(sizeof(char*) * list_length);
        select_menu[0] = "Back";
        print_centered_list(game, select_menu, list_length);
    }

    int key = wgetch(game->main_win->win);
    if (game->context_data.menu_data.setting == 2) {
        if (key == '\t') {
            curs_set(0);
            if (game->config.seed == 0) game->config.seed = time(NULL);
            game->context_data.menu_data.selected = 0;
            game->state = GameSettings;
        } else if (isdigit(key) && game->config.seed <= 9999999999999999) {
            game->config.seed *= 10;
            game->config.seed += key - '0';
        } else if (key == KEY_BACKSPACE && game->config.seed >= 0) {
            game->config.seed /= 10;
        }
    } else if (handle_select_menu(game, key, list_length)) {
        if (game->context_data.menu_data.setting == 0) {
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
            save_config(game);
        }
        game->context_data.menu_data.selected = 0;
        game->state = GameSettings;
    }
    free(select_menu);
}

void game_help(Game* game) {
    char* messages[] = {
        "You are a FROG",
        "",
        "q to exit level",
        "(when playing)",
        "",
        "wsad or arrows",
        "to move"
    };
    for (int i = 0; i < sizeof(messages) / sizeof(char*); i++) {
        mvwprintw(game->main_win->win, i + 1, centerX(game, messages[i]), messages[i]);
    }

    char* back = "> Back <";
    mvwprintw(game->main_win->win, game->main_win->rows * 2 / 3, centerX(game, back), back);
    wrefresh(game->main_win->win);

    int key = wgetch(game->main_win->win);
    if (key == ' ' || key == 'e') {
        game->state = GameMenu;
        game->context_data.menu_data.selected = 2;
    } else if (key == 'q') {
        game->state = GameExit;
    }
}

////////////////
/// GAMEPLAY ///
////////////////

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

void game_play_init(Game* game) {
    game->context_data.game_data.player = create_player(game->main_win->rows - 2, game->main_win->cols / 2);
    game->context_data.game_data.lines = calloc(game->main_win->rows, sizeof(int));
    switch (game->context_data.game_data.level) {
        case 1:
            game->context_data.game_data.goal.x = game->main_win->cols / 2;
            game->context_data.game_data.goal.y = 1;
            break;
        case 2:
            game->context_data.game_data.lines[game->main_win->rows / 2] = 1;
            break;
        default: break;
    }

    game->context_data.game_data.state = Playing;
}

void render_lines(Game* game) {
    for (int i = 1; i < game->main_win->rows - 1; i++) {
        for (int j = 1; j < game->main_win->cols - 1; j++) {
            change_color(game->main_win, game->context_data.game_data.lines[i]);
            mvwprintw(game->main_win->win, i, j, " ");
        }
    }
}

void tutorial_text(Game* game) {
    wcolor_set(game->main_win->win, GRASS_BLACK_COL, NULL);
    switch (game->context_data.game_data.level) {
        case 1:
            char* helps1[] = {"^ collect to", "complete level", "you move with WSAD", " or arrow keys", "this is you", "a FROG", "V"};
            mvwprintw(game->main_win->win, 2, game->main_win->cols / 2, helps1[0]);
            mvwprintw(game->main_win->win, 3, game->main_win->cols / 2, helps1[1]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2, centerX(game, helps1[2]), helps1[2]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 1, centerX(game, helps1[3]), helps1[3]);
            mvwprintw(game->main_win->win, game->main_win->rows - 5, centerX(game, helps1[4]), helps1[4]);
            mvwprintw(game->main_win->win, game->main_win->rows - 4, centerX(game, helps1[5]), helps1[5]);
            mvwprintw(game->main_win->win, game->main_win->rows - 3, centerX(game, helps1[6]), helps1[6]);
            break;
        case 2:
            char* help1 = "^ this is a road";
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 1, game->main_win->cols - (int) strlen(help1) - 2, help1);
            char* help2 = "you need to";
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 2, game->main_win->cols - (int) strlen(help2) - 2, help2);
            char* help3 = "avoid o=o";
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 3, game->main_win->cols - (int) strlen(help3) - 2, help3);
            break;
        default: break;
    }
}

void draw_goal(const Game* game) {
    Goal goal = game->context_data.game_data.goal;
    change_color(game->main_win, game->context_data.game_data.lines[goal.y]);
    mvwprintw(game->main_win->win, goal.y, goal.x, "*");
}

void draw_frog(const Game* game) {
    Player* player = game->context_data.game_data.player;
    change_color(game->main_win, game->context_data.game_data.lines[player->y]);
    mvwprintw(game->main_win->win, player->y, player->x, &game->config.frog);
}

void move_frog(Game* game, int key) {
    Player* player = game->context_data.game_data.player;
    switch (key) {
        case 'w':
            if (player->y > 1) {
                player->y--;
                player->curr_pts++;
            } else player->y = 1;
            break;
        case 's':
            if (player->y < game->main_win->rows - 2) {
                player->y++;
                player->curr_pts--;
            } else player->y = game->main_win->rows - 2;
            break;
        case 'a':
            if (player->x > 1) player->x--;
            else player->x = 1;
            break;
        case 'd':
            if (player->x < game->main_win->cols - 2) player->x++;
            else player->x = game->main_win->cols - 2;
            break;
        default: break;
    }
}

void collision_win(Game* game) {
    Goal goal = game->context_data.game_data.goal;
    Player* player = game->context_data.game_data.player;
    if (goal.x == player->x && goal.y == player->y) {
        game->context_data.game_data.state = PlayingSuccess;
        free(player);
    }
}

void game_play_run(Game* game) {
    clear_win(game->top_win);
    mvwprintw(game->top_win->win, 1, 2, "Level: %d", game->context_data.game_data.level);
    render_lines(game);
    tutorial_text(game);
    draw_goal(game);
    draw_frog(game);
    collision_win(game);
    int key = wgetch(game->main_win->win);
    move_frog(game, key);
    switch (key) {
        case 'q':
            game->state = GameMenu;
            game->context_data.menu_data.selected = 0;
            break;
        default: break;
    }
}

void game_play_success(Game* game) {
    clear_win(game->top_win);
    mvwprintw(game->top_win->win, 1, 2, "Level: %d Success", game->context_data.game_data.level);
    wcolor_set(game->main_win->win, DEFAULT_COL, NULL);
    clear_win(game->main_win);
    wrefresh(game->main_win->win);
}

void game_play(Game* game) {
    switch (game->context_data.game_data.state) {
        case PlayingInit:
            game_play_init(game);
            break;
        case Playing:
            game_play_run(game);
            break;
        case PlayingSuccess:
            game_play_success(game);
            break;
        default: break;
    }
}

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
    init_ncurses();
    Game* game = create_game();
    while (1) {
        clear_win(game->main_win);
        switch (game->state) {
            case GameMenu:
                game_menu(game);
                break;
            case GameHelp:
                game_help(game);
                break;
            case GameSettings:
                game_settings_menu(game);
                break;
            case GameSettingsEdit:
                game_settings_edit(game);
                break;
            case GamePlaying:
                game_play(game);
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
    }
}
