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
        if (line->type == LineRoad && timer_elapsed(&line->spawn_timer, line->next_car)) {
            line->next_car = RA(line->min_next_car, line->max_next_car);
            timer_start(&line->spawn_timer);
            spawn_car_on_line(win, cars, line);
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
