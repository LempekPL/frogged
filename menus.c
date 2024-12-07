#include "menus.h"

#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "util.h"

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

void run_game_main_menu(Game* game) {
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

void run_game_help(Game* game) {
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

void run_game_settings_menu(Game* game) {
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

void run_game_settings_edit(Game* game) {
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
            redraw_game_border(game);
            save_config(game);
        }
        game->context_data.menu_data.selected = 0;
        game->state = GameSettings;
    }
    free(select_menu);
}
