#include "levels.h"
#include "util.h"
#include "game_loop.h"
#include <stdlib.h>


void tutorial1(const Game* game) {
    char* helps[] = {"^ collect to", "complete level", "you move with WSAD", " or arrow keys", "this is you", "a FROG", "V"};
    int half_width = game->main_win->cols / 2;
    int height = game->main_win->rows;
    print_main(game, 2, half_width, helps[0]);
    print_main(game, 3, half_width, helps[1]);
    print_main(game, height / 2, centerX(game, helps[2]), helps[2]);
    print_main(game, height / 2 + 1, centerX(game, helps[3]), helps[3]);
    print_main(game, height - 5, centerX(game, helps[4]), helps[4]);
    print_main(game, height - 4, centerX(game, helps[5]), helps[5]);
    print_main(game, height - 3, centerX(game, helps[6]), helps[6]);
}

void tutorial2(const Game* game) {
    char* helps2[] = {"^ road", "you need to", "avoid    ", "o=o"};
    int half_height = game->main_win->rows / 2;
    print_main(game, half_height + 1, rightX(game, helps2[0]) - 2, helps2[0]);
    print_main(game, half_height + 2, rightX(game, helps2[1]) - 2, helps2[1]);
    print_main(game, half_height + 3, rightX(game, helps2[2]) - 1, helps2[2]);
    wcolor_set(game->main_win->win, ROAD_RED_COL, NULL);
    print_main(game, half_height + 3, rightX(game, helps2[3]) - 1, helps2[3]);
}

void tutorial3(const Game* game) {
    char* helps3[] = {"this is stopper", "o=o", "it stops", "in front of you", "this car can still", "kill you if you", "run into it"};
    int half_height = game->main_win->rows / 2;
    print_main(game, half_height + 1, centerX(game, helps3[0]) + 5, helps3[0]);
    print_main(game, half_height + 2, centerX(game, helps3[2]) + 8, helps3[2]);
    print_main(game, half_height + 3, centerX(game, helps3[3]) + 5, helps3[3]);
    print_main(game, half_height + 4, centerX(game, helps3[4]) + 5, helps3[4]);
    print_main(game, half_height + 5, centerX(game, helps3[5]) + 5, helps3[5]);
    print_main(game, half_height + 6, centerX(game, helps3[6]) + 5, helps3[6]);
    wcolor_set(game->main_win->win, ROAD_YELLOW_COL, NULL);
    print_main(game, half_height + 2, centerX(game, helps3[1]) + 1, helps3[1]);
}

void tutorial4(const Game* game) {
    char* helps4[] = {"now try it without", "any help :)"};
    print_main(game, 4, centerX(game, helps4[0]), helps4[0]);
    print_main(game, 5, centerX(game, helps4[1]), helps4[1]);
}

void tutorial6(const Game* game) {
    char* helps6[] = {"^ water", "it kills you", "use logs", "#", "to move"};
    int half_height = game->main_win->rows / 2;
    print_main(game, half_height + 1, 2, helps6[0]);
    print_main(game, half_height + 2, 2, helps6[1]);
    print_main(game, half_height + 1, rightX(game, helps6[2]) - 1, helps6[2]);
    print_main(game, half_height + 2, rightX(game, helps6[4]) - 1, helps6[4]);
    wcolor_set(game->main_win->win, WATER_COL, NULL);
    print_main(game, half_height + 2, rightX(game, helps6[3]) - (int) strlen(helps6[4]) - 2, helps6[3]);
}

void draw_tutorial_text(const Game* game) {
    wcolor_set(game->main_win->win, GRASS_BLACK_COL, NULL);
    switch (game->context_data.game_data.level) {
        case 1: tutorial1(game);
            break;
        case 2: tutorial2(game);
            break;
        case 3: tutorial3(game);
            break;
        case 4: tutorial4(game);
            break;
        case 6: tutorial6(game);
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

void level2(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Line line = new_line_car(LineRoad, game->main_win->rows / 2, ToLeft, 200, 100, 1000, 6000, 0);
    replace_self_lines(game_data->lines, &line);
}

void level3(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Line line = new_line_car(LineRoad, game->main_win->rows / 2, ToLeft, 200, 100, 1000, 6000, 1);
    replace_self_lines(game_data->lines, &line);
}

void level4(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Line line = new_line_car(LineRoad, game->main_win->rows / 2 - 1, ToLeft, 200, 100, 1000, 6000, .2);
    replace_self_lines(game_data->lines, &line);
    line.line_data.car.stopper_chance = .5;
    line.y = game->main_win->rows / 2 + 1;
    replace_self_lines(game_data->lines, &line);
}

void level5(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Line line = new_line_car(LineRoad, game->main_win->rows / 2, ToLeft, 100, 100, 2000, 6000, .2);
    replace_self_lines(game_data->lines, &line);

    line.line_data.car.stopper_chance = .25;
    line.y = game->main_win->rows / 2 + 1;
    replace_self_lines(game_data->lines, &line);

    line.line_data.car.cars_direction = ToRight;
    line.line_data.car.max_next_car = 3000;
    line.y = game->main_win->rows / 2 - 1;
    replace_self_lines(game_data->lines, &line);
}

void level6(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Line line = new_line(LineWater, game->main_win->rows / 2);
    int* logs = malloc(sizeof(int) * 3);
    logs[0] = game->main_win->cols / 2;
    logs[1] = game->main_win->cols / 2 + 1;
    logs[2] = game->main_win->cols / 2 - 1;
    line.line_data.water.logs = logs;
    line.line_data.water.logs_amount = 3;
    replace_self_lines(game_data->lines, &line);
}

void level7(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Line line = new_line_car(LineRoad, game->main_win->rows / 2 + 1, ToLeft, 100, 100, 2000, 6000, .2);
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
}

void level8(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Line line = new_line_car(LineRoad, game->main_win->rows / 2, ToLeft, 200, 100, 1000, 6000, .25);
    for (int i = -6; i <= 6; i++) {
        line.y = game->main_win->rows / 2 - i;
        replace_self_lines(game_data->lines, &line);
    }
}

void level9(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Line line = new_line_car(LineRoad, game->main_win->rows / 2, ToLeft, 200, 100, 1000, 6000, 1);
}

void level10(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    Line line = new_line_car(LineRoad, game->main_win->rows / 2, ToLeft, 200, 100, 1000, 6000, 1);
}

void game_levels_init(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    game_data->player = create_player(game->main_win->rows - 2, game->main_win->cols / 2, game->config.frog, game->config.cooldown);
    game_data->cars = new_cars(2);
    game_data->lines = generate_default_lines(game->main_win->rows);
    game_data->goal.x = game->main_win->cols / 2;
    game_data->goal.y = 1;
    switch (game_data->level) {
        case 2: level2(game);
            break;
        case 3: level3(game);
            break;
        case 4: level4(game);
            break;
        case 5: level5(game);
            break;
        case 6: level6(game);
            break;
        case 7: level7(game);
            break;
        case 8: level8(game);
            break;
        case 9:
            break;
        case 10:
            break;
        default: break;
    }
    game_data->state = PlayingLevels;
}

void game_levels_draw(const Game* game, const GameData* game_data) {
    draw_lines(game->main_win, game_data->lines);
    draw_tutorial_text(game);
    draw_goal(game);
    draw_player(game->main_win, game_data->player, game_data->lines);
    draw_cars(game->main_win, game_data->cars);
}

void game_levels_handle_collision(Game* game, GameData* game_data) {
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
}

void game_levels_play(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    char level_name[16];
    sprintf(level_name, " Level: %d", game_data->level);
    print_top(game, level_name, 0);
    game_levels_draw(game, game_data);
    spawn_cars_randomly(game->main_win, game_data->cars, game_data->lines);
    int key = wgetch(game->main_win->win);
    move_player(game_data->player, key, game->main_win->cols - 2, game->main_win->rows - 2);
    move_cars(game->main_win, game_data->player, game_data->cars, game->main_win->cols);
    game_levels_handle_collision(game, game_data);
    if (key == 'q') {
        game->state = GameMenu;
        game->context_data.menu_data.selected = 0;
    }
}
