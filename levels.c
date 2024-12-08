#include "levels.h"
#include "util.h"
#include "game_loop.h"
#include <stdlib.h>

void draw_tutorial_text(const Game* game) {
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
            char* helps2[] = {"^ road", "you need to", "avoid    ", "o=o"};
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 1, rightX(game, helps2[0]) - 2, helps2[0]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 2, rightX(game, helps2[1]) - 2, helps2[1]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 3, rightX(game, helps2[2]) - 1, helps2[2]);
            wcolor_set(game->main_win->win, ROAD_RED_COL, NULL);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 3, rightX(game, helps2[3]) - 1, helps2[3]);
            break;
        case 3:
            char* helps3[] = {
                "this is stopper", "o=o", "it stops", "in front of you", "this car can still", "kill you if you", "run into it"
            };
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 1, centerX(game, helps3[0]) + 5, helps3[0]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 2, centerX(game, helps3[2]) + 8, helps3[2]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 3, centerX(game, helps3[3]) + 5, helps3[3]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 4, centerX(game, helps3[4]) + 5, helps3[4]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 5, centerX(game, helps3[5]) + 5, helps3[5]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 6, centerX(game, helps3[6]) + 5, helps3[6]);
            wcolor_set(game->main_win->win, ROAD_YELLOW_COL, NULL);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 2, centerX(game, helps3[1]) + 1, helps3[1]);
            break;
        case 4:
            char* helps4[] = {"now try it without", "any help :)"};
            mvwprintw(game->main_win->win, 4, centerX(game, helps4[0]), helps4[0]);
            mvwprintw(game->main_win->win, 5, centerX(game, helps4[1]), helps4[1]);
        break;
        case 6:
            char* helps6[] = {"^ water", "it kills you", "use logs", "#", "to move"};
            mvwprintw(game->main_win->win, game->main_win->rows / 2+1, 2, helps6[0]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2+2, 2, helps6[1]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2+1, rightX(game, helps6[2])-1, helps6[2]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2+2, rightX(game, helps6[4])-1, helps6[4]);
            wcolor_set(game->main_win->win, WATER_COL, NULL);
            mvwprintw(game->main_win->win, game->main_win->rows / 2+2, rightX(game, helps6[3])-(int)strlen(helps6[4])-2, helps6[3]);
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

void game_levels_init(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    game_data->player = create_player(game->main_win->rows - 2, game->main_win->cols / 2, game->config.frog, game->config.cooldown);
    game_data->cars = new_cars(2);
    game_data->lines = generate_default_lines(game->main_win->rows);
    game_data->goal.x = game->main_win->cols / 2;
    game_data->goal.y = 1;
    Line line = new_line_car(LineRoad, game->main_win->rows / 2, ToLeft, 200, 100, 1000, 6000);
    switch (game_data->level) {
        case 1:
            break;
        case 2:
            replace_self_lines(game_data->lines, &line);
            break;
        case 3:
            line.line_data.car.stopper_chance = 1;
            replace_self_lines(game_data->lines, &line);
            break;
        case 4:
            line.line_data.car.stopper_chance = .2;
            line.y = game->main_win->rows / 2 - 1;
            replace_self_lines(game_data->lines, &line);

            line.line_data.car.stopper_chance = .5;
            line.y = game->main_win->rows / 2 + 1;
            replace_self_lines(game_data->lines, &line);
            break;
        case 5:
            line.line_data.car.line_speed_limit = 100;
            line.line_data.car.min_next_car = 2000;
            line.line_data.car.max_next_car = 6000;
            line.line_data.car.line_speed_limit = 100;
            line.line_data.car.stopper_chance = .2;
            line.y = game->main_win->rows / 2 + 1;
            replace_self_lines(game_data->lines, &line);

            line.line_data.car.stopper_chance = .25;
            line.y = game->main_win->rows / 2;
            replace_self_lines(game_data->lines, &line);

            line.line_data.car.cars_direction = ToRight;
            line.line_data.car.max_next_car = 3000;
            line.y = game->main_win->rows / 2 - 1;
            replace_self_lines(game_data->lines, &line);
            break;
        case 6:
            line = new_line(LineWater, game->main_win->rows / 2);
            int* logs = malloc(sizeof(int) * 3);
            logs[0] = game->main_win->cols / 2;
            logs[1] = game->main_win->cols / 2 + 1;
            logs[2] = game->main_win->cols / 2 - 1;
            line.line_data.water.logs = logs;
            line.line_data.water.logs_amount = 3;
            replace_self_lines(game_data->lines, &line);
            break;
        case 7:
            line.line_data.car.line_speed_limit = 100;
            line.line_data.car.min_next_car = 2000;
            line.line_data.car.max_next_car = 6000;
            line.line_data.car.line_speed_limit = 100;
            line.line_data.car.stopper_chance = .2;
            line.y = game->main_win->rows / 2 + 1;
            replace_self_lines(game_data->lines, &line);

            line.line_data.car.stopper_chance = .25;
            line.y = game->main_win->rows / 2;
            replace_self_lines(game_data->lines, &line);

            line.line_data.car.cars_direction = ToRight;
            line.line_data.car.max_next_car = 3000;
            line.y = game->main_win->rows / 2 - 1;
            replace_self_lines(game_data->lines, &line);

            line = new_line(LineWater, game->main_win->rows / 2);
            int* logs2 = malloc(sizeof(int) * 3);
            logs2[0] = game->main_win->cols / 2;
            logs2[1] = game->main_win->cols / 2 + 1;
            logs2[2] = game->main_win->cols / 2 - 1;
            line.line_data.water.logs = logs2;
            line.line_data.water.logs_amount = 3;
            replace_self_lines(game_data->lines, &line);
            break;
        case 8:
            line.line_data.car.line_speed_limit = 100;
            line.line_data.car.min_next_car = 2000;
            line.line_data.car.max_next_car = 6000;
            line.line_data.car.line_speed_limit = 200;
            line.line_data.car.stopper_chance = .2;
            line.y = game->main_win->rows / 2 - 6;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 - 5;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 - 4;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 - 3;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 - 2;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 - 1;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 + 1;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 + 2;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 + 3;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 + 4;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 + 5;
            replace_self_lines(game_data->lines, &line);
            line.y = game->main_win->rows / 2 + 6;
            replace_self_lines(game_data->lines, &line);
            break;
        case 9:
            break;
        case 10:
            break;
        default: break;
    }
    game_data->state = PlayingLevels;
}

void game_levels_play(Game* game) {
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
    move_cars(game->main_win, game_data->player, game_data->cars, game->main_win->cols);
    PlayerCollision collision_type = collision_player(game_data);
    switch (collision_type) {
        case PlayerTouchGoal:
            if (game->config.completed < game->context_data.game_data.level) {
                game->config.completed = game->context_data.game_data.level;
                save_config(&game->config, "config.txt");
            }
            game_data->state = PlayingSuccess;
            game_data->end_select = 0;
            free_game_data(game_data);
            break;
        case PlayerTouchDeath:
            game_data->state = PlayingKilled;
            game_data->end_select = 0;
            free_game_data(game_data);
            break;
        default: break;
    }

    if (key == 'q') {
        game->state = GameMenu;
        game->context_data.menu_data.selected = 0;
    }
}
