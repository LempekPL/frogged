#include "endless.h"
#include "util.h"
#include "game_loop.h"
#include <stdlib.h>

Lines* generate_random_lines(int height) {
    Lines* new_lines_vec = new_lines(height);
    for (int i = 1; i < height; i++) {
        LineType line_type = LineGrass;
        if (RA(0,1) == 1) {
            line_type = LineRoad;
        }
        LineDirection line_dir = DirToLeft;
        if (RA(0,1) == 1) {
            line_dir = DirToRight;
        }
        Line line = new_line_car(line_type, i, line_dir, RA(100, 500), RA(0, 100), 1000, 6000, .25);
        add_line(new_lines_vec, &line);
    }
    return new_lines_vec;
}

void game_endless_init(Game* game) {
    GameData* game_data = &game->context_data.game_data;
    game_data->player = create_player(game->main_win->rows - 2, game->main_win->cols / 2, game->config.frog, game->config.cooldown);
    game_data->cars = new_cars(2);
    // game_data->lines = generate_default_lines(game->main_win->rows);
    game_data->lines = generate_random_lines(game->main_win->rows+1);
    // Line line = new_line(LineGrass, game->main_win->rows-2);
    // replace_self_lines(game_data->lines, &line);
    // game_data->lines = lines;
    game_data->state = PlayingEndless;
}

void handle_endless_collision(GameData* game_data) {
    PlayerCollision collision_type = collision_player(game_data);
    switch (collision_type) {
        case PlayerTouchGoal:
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

void game_endless_play(Game* game) {
    GameData* game_data = &game->context_data.game_data;

    char level_name[16];
    sprintf(level_name, " Endless | Pts: %d", game_data->player->curr_pts);
    print_top(game, level_name, 0);

    draw_lines(game->main_win, game_data->lines);
    draw_player(game->main_win, game_data->player, game_data->lines);
    draw_cars(game->main_win, game_data->cars);

    spawn_cars_randomly(game->main_win, game_data->cars, game_data->lines);
    int key = wgetch(game->main_win->win);
    move_player(game_data->player, key, game->main_win->cols - 2, game->main_win->rows - 2);
    if (game_data->player->y == 1) {
        game_data->player->y = game->main_win->rows-2;
        // free_cars(game_data->cars);
        // game_data->cars = new_cars(2);
        // free_lines(game_data->lines);
        // game_data->lines = generate_random_lines(game->main_win->rows-1);
    }
    move_cars(game->main_win, game_data->player, game_data->cars, game->main_win->cols);
    handle_endless_collision(game_data);

    if (key == 'q') {
        game->state = GameMenu;
        game->context_data.menu_data.selected = 0;
    }
}
