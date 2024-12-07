#ifndef UTIL_H
#define UTIL_H
#include <string.h>

#define RA(min, max) ( (min) + rand() % ((max) - (min) + 1) )
#define Clamp(value, min, max) ( value < min ? min : value > max ? max : value )
#define centerX(game, string) (game->main_win->cols / 2 - (int) strlen(string) / 2)

int digit_amount(long long n);

#endif //UTIL_H
