#ifndef GAME_LOOP_H
#define GAME_LOOP_H
#include "game.h"

void run_game_play(Game*);
void collision(Game* game);
void spawn_cars_randomly(const Win* win, Cars* cars, const Lines* lines);

#endif //GAME_LOOP_H
