#include "levels.h"
#include "util.h"
#include "game_loop.h"
#include <stdlib.h>


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
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 1, centerX(game, helps2[0]) + 6, helps2[0]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 2, centerX(game, helps2[1]) + 6, helps2[1]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 3, centerX(game, helps2[2]) + 5, helps2[2]);
            wcolor_set(game->main_win->win, ROAD_RED_COL, NULL);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 3, centerX(game, helps2[3]) + 9, helps2[3]);
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
    game_data->player = create_player(game->main_win->rows - 2, game->main_win->cols / 2, game->config.frog);
    game_data->cars = new_cars(2);
    game_data->lines = generate_default_lines(game->main_win->rows);
    switch (game_data->level) {
        case 1:
            game_data->goal.x = game->main_win->cols / 2;
        game_data->goal.y = 1;
        break;
        case 2:
            Line line1 = new_line_ext(LineRoad, game->main_win->rows / 2, DirToLeft, 200, RA(0, 100), 1000, 6000);
        replace_at_lines(game_data->lines, &line1, game->main_win->rows / 2);
        break;
        case 3:
            Line line2 = new_line_ext(LineRoad, game->main_win->rows / 2, DirToRight, 200, RA(0, 100), 1000, 6000);
        line2.stopper_chance = 1;
        replace_at_lines(game_data->lines, &line2, game->main_win->rows / 2);
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
    collision(game);
    if (key == 'q') {
        game->state = GameMenu;
        game->context_data.menu_data.selected = 0;
    }
}