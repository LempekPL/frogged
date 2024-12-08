#include "game_loop.h"
#include "window.h"
#include "util.h"
#include <stdlib.h>
#include "menus.h"

void game_play_init(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    game_data->player = create_player(game->main_win->rows - 2, game->main_win->cols / 2, game->config.frog);
    game_data->cars = new_cars(2);
    game_data->lines = generate_default_lines(game->main_win->rows);
    switch (game_data->level) {
        case 1:
            game_data->goal.x = game->main_win->cols / 2;
            game_data->goal.y = 1;
            break;
        case 2:
            Line line = new_line_ext(LineRoad, game->main_win->rows / 2, DirToLeft, RA(0, 100), 1000, 6000);
            replace_at_lines(game_data->lines, &line, game->main_win->rows / 2);
            break;
        default: break;
    }
    game_data->state = Playing;
}

void free_game_data(GameData* game_data) {
    free(game_data->player);
    free_cars(game_data->cars);
    free_lines(game_data->lines);
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
    Line* line = ptr_at_lines(game->context_data.game_data.lines, 1);
    change_color(game->main_win, line->type);
    mvwprintw(game->main_win->win, goal.y, goal.x, "*");
}

void collision(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Goal goal = game_data->goal;
    Player* player = game_data->player;
    if (goal.x == player->x && goal.y == player->y) {
        game->context_data.game_data.state = PlayingSuccess;
        game->context_data.game_data.end_select = 0;
        free_game_data(game_data);
    } else {
        for (int i = 0; i < game_data->cars->size; i++) {
            Car* car = ptr_at_cars(game_data->cars, i);
            if (car->y == player->y && car->x <= player->x && car->x + 3 > player->x) {
                game->context_data.game_data.state = PlayingKilled;
                game->context_data.game_data.end_select = 0;
                free_game_data(game_data);
            }
        }
    }
}

void spawn_cars_randomly(Win* win, Cars* cars, Lines* lines) {
    for (int i = 0; i < lines->size; i++) {
        Line* line = ptr_at_lines(lines, i);
        if (line->type == LineRoad && timer_elapsed(&line->spawn_timer, line->car_freq)) {
            mvwprintw(win->win, 1, 1, "%d", line->car_freq);
            line->car_freq = RA(line->min_random, line->max_random);
            timer_start(&line->spawn_timer);
            Car car = spawn_car_on_line(win, *line, 200, CarEnemy);
            add_car(cars, &car);
        }
    }
}

void game_play_run(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    char level_name[16];
    sprintf(level_name, " Level: %d", game_data->level);
    print_top(game, level_name, 0);
    draw_lines(game->main_win, game_data->lines);
    draw_tutorial_text(game);
    draw_goal(game);
    draw_player(game->main_win, game_data->player, game_data->lines);
    draw_cars(game->main_win, game_data->cars);

    spawn_cars_randomly(game->main_win, game_data->cars, game_data->lines);

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
        case PlayingKilled:
            run_game_killed_menu(game);
            break;
        default: break;
    }
}
