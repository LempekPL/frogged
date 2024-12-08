#include "game_loop.h"
#include "window.h"
#include "util.h"
#include "levels.h"
#include "menus.h"
#include "endless.h"
#include <stdlib.h>


void free_game_data(GameData* game_data) {
    free(game_data->player);
    free_cars(game_data->cars);
    free_lines(game_data->lines);
}

void spawn_cars_randomly(const Win* win, Cars* cars, const Lines* lines) {
    for (int i = 0; i < lines->size; i++) {
        Line* line = ptr_at_lines(lines, i);
        if (line->type == LineRoad) {
            LineCarData* line_data = &line->line_data.car;
            if (timer_elapsed(&line_data->spawn_timer, line_data->next_car)) {
                line_data->next_car = RA(line_data->min_next_car, line_data->max_next_car);
                timer_start(&line_data->spawn_timer);
                spawn_car_on_line(win, cars, line);
            }
        }
    }
}

void run_game_play(Game* game) {
    switch (game->context_data.game_data.state) {
        case PlayingInitLevels:
            game_levels_init(game);
            break;
        case PlayingLevels:
            game_levels_play(game);
            break;
        case PlayingInitEndless:
            game_endless_init(game);
            break;
        case PlayingEndless:
            game_endless_play(game);
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
