#ifndef UTIL_H
#define UTIL_H
#include <string.h> // this needs to be here beacause of centerX
#include "game.h"

#define RA(min, max) ( (min) + rand() % ((max) - (min) + 1) )
#define Clamp(value, min, max) ( value < min ? min : value > max ? max : value )
#define centerX(game, string) (game->main_win->cols / 2 - (int) strlen(string) / 2)
#define rightX(game, string) (game->main_win->cols - 1 - (int) strlen(string))

int digit_amount(long long);
void change_color(const Win* win, LineType line_type);
void print_top(Game*, char* string, int centered);

#endif //UTIL_H
