#include "menus.h"
#include "config.h"
#include "util.h"
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

void print_centered_list(Game* game, int selected, char** string_list, int list_length) {
    for (int i = 0; i < list_length; i++) {
        int offset = i == selected ? -2 : 0;
        int centeredY = game->main_win->rows / 2 - list_length / 2 + i;
        mvwprintw(game->main_win->win, centeredY, centerX(game, string_list[i]) + offset,
                  i == selected ? "> %s <" : "%s", string_list[i]);
    }
    wrefresh(game->main_win->win);
}

int handle_select_menu(int* selection_value, int key, int max) {
    switch (key) {
        case 'w':
        case KEY_UP:
            *selection_value -= 1;
            break;
        case 's':
        case KEY_DOWN:
            *selection_value += 1;
            break;
        case 'e':
        case ' ':
            return 1;
        default: break;
    }
    *selection_value = Clamp(*selection_value, 0, max - 1);
    return 0;
}

void run_game_main_menu(Game* game) {
    wcolor_set(game->main_win->win, DEFAULT_COL, NULL);
    print_top(game, "FROGGED", 1);
    char* select_menu[5] = {"Start Game", "Levels", "How to", "Settings", "Exit"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, game->context_data.menu_data.selected, select_menu, list_length);

    int key = wgetch(game->main_win->win);
    if (handle_select_menu(&game->context_data.menu_data.selected, key, list_length)) {
        switch (game->context_data.menu_data.selected) {
            case 0:
                game->state = GameMenuStart;
                break;
            case 2:
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

void run_game_start_menu(Game* game) {
    wcolor_set(game->main_win->win, DEFAULT_COL, NULL);
    char* select_menu[4] = {"New Game", "Continue", "Endless", "Back"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, game->context_data.menu_data.selected, select_menu, list_length);

    int key = wgetch(game->main_win->win);
    if (handle_select_menu(&game->context_data.menu_data.selected, key, list_length)) {
        switch (game->context_data.menu_data.selected) {
            case 0:
                game->state = GamePlaying;
                game->context_data.game_data.level = 1;
                game->config.completed = 1;
                save_config(&game->config, "config.txt");
                game->context_data.game_data.state = PlayingInitLevels;
                break;
            case 1:
                game->state = GamePlaying;
                game->context_data.game_data.level = game->config.completed;
                game->context_data.game_data.state = PlayingInitLevels;
                break;
            case 2:
                game->state = GamePlaying;
                game->context_data.game_data.state = PlayingInitEndless;
                break;
            case 3:
                game->context_data.menu_data.selected = 0;
                game->state = GameMenu;
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
        "to move",
        "",
        "collect '*' to win",
        "",
        "> Back <"
    };
    for (int i = 0; i < sizeof(messages) / sizeof(char*); i++) {
        mvwprintw(game->main_win->win, i + 1, centerX(game, messages[i]), messages[i]);
    }

    int key = wgetch(game->main_win->win);
    if (key == 'q' || key == ' ' || key == 'e') {
        game->state = GameMenu;
        game->context_data.menu_data.selected = 2;
    }
}

void run_game_settings_menu(Game* game) {
    char* select_menu[5] = {"Back", "Border", "Size", "Seed", "Frog"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, game->context_data.menu_data.selected, select_menu, list_length);

    int key = wgetch(game->main_win->win);
    if (handle_select_menu(&game->context_data.menu_data.selected, key, list_length)) {
        switch (game->context_data.menu_data.selected) {
            case 0:
                game->context_data.menu_data.selected = 3;
                game->state = GameMenu;
                break;
            default:
                game->context_data.menu_data.setting = game->context_data.menu_data.selected;
                game->context_data.menu_data.selected = 0;
                game->state = GameSettingsEdit;
                break;
        }
    }
}

void settings_border(Game* game) {
    char* select_menu[4] = {"Simple", "Clean", "Wrapped", "Return without saving"};
    int select_menu_length = sizeof(select_menu) / sizeof(select_menu[0]);
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
    print_centered_list(game, game->context_data.menu_data.selected, select_menu, select_menu_length);

    int key = wgetch(game->main_win->win);
    if (handle_select_menu(&game->context_data.menu_data.selected, key, select_menu_length)) {
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
        save_config(&game->config, "config.txt");
        game->context_data.menu_data.selected = 0;
        game->state = GameSettings;
    }
}

void settings_size(Game* game) {
}

void settings_seed(Game* game) {
    char* messages[] = {
        "Selecting won't save to file",
        "Leaving 0 will",
        "use time(NULL) as seed",
        "Click E to accept"
    }; // , "Click TAB to cancel"
    mvwprintw(game->main_win->win, 1, centerX(game, messages[0]), messages[0]);
    mvwprintw(game->main_win->win, 3, centerX(game, messages[1]), messages[1]);
    mvwprintw(game->main_win->win, 4, centerX(game, messages[2]), messages[2]);
    mvwprintw(game->main_win->win, 5, centerX(game, messages[3]), messages[3]);

    mvwprintw(game->main_win->win, 10, game->main_win->cols / 2 - 10, "> %lld", game->config.seed);
    mvwprintw(game->main_win->win, 10, game->main_win->cols / 2 + 10, "<");
    wmove(game->main_win->win, 10, game->main_win->cols / 2 + digit_amount(game->config.seed) - 8);
    curs_set(1);
    int key = wgetch(game->main_win->win);
    if (key == 'e' || key == 'q' || key == ' ') {
        curs_set(0);
        if (game->config.seed == 0) game->config.seed = time(NULL);
        game->context_data.menu_data.selected = 0;
        game->state = GameSettings;
        srand(game->config.seed);
        // } else if (key == '\t') {
        //     curs_set(0);
        //     if (game->config.seed == 0) game->config.seed = time(NULL);
        //     game->context_data.menu_data.selected = 0;
        //     game->state = GameSettings;
    } else if (isdigit(key) && game->config.seed <= 9999999999999999) {
        game->config.seed *= 10;
        game->config.seed += key - '0';
    } else if (key == KEY_BACKSPACE && game->config.seed >= 0) {
        game->config.seed /= 10;
    }
}

void settings_frog(Game* game) {
    char* messages[] = {"Type any alphanumeric", "character to set frog"};
    mvwprintw(game->main_win->win, 2, centerX(game, messages[0]), messages[0]);
    mvwprintw(game->main_win->win, 3, centerX(game, messages[1]), messages[1]);
    char* current = "Current:";
    mvwprintw(game->main_win->win, 6, centerX(game, current) - 2, "%s %c", current, game->config.frog);

    int key = wgetch(game->main_win->win);
    if (key == '\t') {
        game->context_data.menu_data.selected = 0;
        game->state = GameSettings;
    } else if (isalnum(key)) {
        game->config.frog = (char) key;
        save_config(&game->config, "config.txt");
        game->context_data.menu_data.selected = 0;
        game->state = GameSettings;
    }
}

void run_game_settings_edit(Game* game) {
    switch (game->context_data.menu_data.setting) {
        case 1:
            settings_border(game);
            break;
        case 2:
            settings_size(game);
            break;
        case 3:
            settings_seed(game);
            break;
        case 4:
            settings_frog(game);
            break;
    }
}

void run_game_success_menu(Game* game) {
    char level_name[26];
    sprintf(level_name, " Level: %d Completed", game->context_data.game_data.level);
    print_top(game, level_name, 0);
    wcolor_set(game->main_win->win, DEFAULT_COL, NULL);
    char* message = "YOU DID IT!!!";
    mvwprintw(game->main_win->win, 4, centerX(game, message), message);
    char* select_menu[4] = {"Next Level", "Levels", "Main Menu", "Exit"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, game->context_data.game_data.end_select, select_menu, list_length);

    int key = wgetch(game->main_win->win);
    if (handle_select_menu(&game->context_data.game_data.end_select, key, list_length)) {
        switch (game->context_data.game_data.end_select) {
            case 0:
                game->state = GamePlaying;
                game->context_data.game_data.level += 1;
                game->context_data.game_data.state = PlayingInitLevels;
                break;
            case 1:
                break;
            case 2:
                game->context_data.menu_data.selected = 0;
                game->state = GameMenu;
                break;
            case 3:
                game->state = GameExit;
                break;
        }
    }
}

void run_game_killed_menu(Game* game) {
    char level_name[24];
    sprintf(level_name, " Level: %d Failed", game->context_data.game_data.level);
    print_top(game, level_name, 0);
    wcolor_set(game->main_win->win, DEFAULT_COL, NULL);
    char* message = "YOU FAILED!!!";
    mvwprintw(game->main_win->win, 4, centerX(game, message), message);
    char* select_menu[4] = {"Try again", "Levels", "Main Menu", "Exit"};
    int list_length = sizeof(select_menu) / sizeof(select_menu[0]);
    print_centered_list(game, game->context_data.game_data.end_select, select_menu, list_length);

    int key = wgetch(game->main_win->win);
    if (handle_select_menu(&game->context_data.game_data.end_select, key, list_length)) {
        switch (game->context_data.game_data.end_select) {
            case 0:
                game->state = GamePlaying;
                game->context_data.game_data.state = PlayingInitLevels;
                break;
            case 1:
                break;
            case 2:
                game->context_data.menu_data.selected = 0;
                game->state = GameMenu;
                break;
            case 3:
                game->state = GameExit;
                break;
        }
    }
}
