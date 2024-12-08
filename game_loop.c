#include "game_loop.h"
#include "window.h"
#include "util.h"
#include <stdlib.h>
#include "menus.h"

void game_play_init(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    game_data->player = create_player(game->main_win->rows - 2, game->main_win->cols / 2, game->config.frog);
    game_data->cars = new_cars(2);
    game_data->lines = calloc(game->main_win->rows, sizeof(int));
    switch (game_data->level) {
        case 1:
            game_data->goal.x = game->main_win->cols / 2;
            game_data->goal.y = 1;
            break;
        case 2:
            game_data->lines[game->main_win->rows / 2] = 1;
            Car car = new_car(1, game->main_win->rows / 2, 200, CarEnemy, ToRight);
            add_car(game_data->cars, &car);
            break;
        default: break;
    }
    game_data->state = Playing;
}

void draw_game_lines(const Game* game) {
    for (int i = 1; i < game->main_win->rows - 1; i++) {
        for (int j = 1; j < game->main_win->cols - 1; j++) {
            change_color(game->main_win, game->context_data.game_data.lines[i]);
            mvwprintw(game->main_win->win, i, j, " ");
        }
    }
}

void draw_tutorial_text(Game* game) {
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
            char* helps2[] = {"^ this is a road", "you need to", "avoid ", "o=o"};
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 1, game->main_win->cols - (int) strlen(helps2[0]) - 2, helps2[0]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 2, game->main_win->cols - (int) strlen(helps2[1]) - 2, helps2[1]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 3, game->main_win->cols - (int) strlen(helps2[2]) - 2 - 3, helps2[2]);
            wcolor_set(game->main_win->win, ROAD_RED_COL, NULL);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 3, game->main_win->cols - (int) strlen(helps2[3]) - 2, helps2[3]);
            break;
        default: break;
    }
}

void draw_goal(const Game* game) {
    Goal goal = game->context_data.game_data.goal;
    change_color(game->main_win, game->context_data.game_data.lines[goal.y]);
    mvwprintw(game->main_win->win, goal.y, goal.x, "*");
}

void collision(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Goal goal = game_data->goal;
    Player* player = game_data->player;
    if (goal.x == player->x && goal.y == player->y) {
        game->context_data.game_data.state = PlayingSuccess;
        game->context_data.game_data.end_select = 0;
        free(player);
        free_cars(game_data->cars);
        free(game_data->lines);
    }
}

void game_play_run(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    char level_name[16];
    sprintf(level_name, " Level: %d", game_data->level);
    print_top(game, level_name, 0);
    draw_game_lines(game);
    draw_tutorial_text(game);
    draw_goal(game);
    change_color(game->main_win, game->context_data.game_data.lines[game_data->player->y]);
    draw_player(game->main_win, game_data->player);
    draw_cars(game->main_win, game_data->cars);

    int key = wgetch(game->main_win->win);
    move_player(game_data->player, key, game->main_win->cols - 2, game->main_win->rows - 2);
    move_cars(game_data->cars, game->main_win->cols);
    collision(game);
    if (key == 'q') {
        game->state = GameMenu;
        game->context_data.menu_data.selected = 0;
    }
}

void run_game_play(Game* game) {
    switch (game->context_data.game_data.state) {
        case PlayingInit:
            game_play_init(game);
            break;
        case Playing:
            game_play_run(game);
            break;
        case PlayingSuccess:
            run_game_success_menu(game);
            break;
        default: break;
    }
}