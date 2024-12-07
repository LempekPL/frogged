#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "settings.h"
#include "util.h"
#include "config.h"

void redraw_game_border(const Game* game) {
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
    redraw_game_border(game);
    return game;
}

void run_game_exit(Game* game) {
    delwin(game->top_win->win);
    delwin(game->main_win->win);
    delwin(game->bottom_win->win);
    free(game->top_win);
    free(game->main_win);
    free(game->bottom_win);
    free(game);
    endwin();
}