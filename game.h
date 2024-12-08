#ifndef GAME_H
#define GAME_H
#include "window.h"
#include "player.h"
#include "config.h"
#include "car.h"

typedef enum {
    GameExit,
    GameMenu,
    GameHelp,
    GameSettings,
    GameSettingsEdit,
    GamePlaying,
} GameState;

typedef struct {
    int selected;
    int setting;
} MenuData;

typedef enum {
    PlayingInit,
    Playing,
    PlayingKilled,
    PlayingSuccess,
} PlayingState;

typedef struct {
    int x, y;
} Goal;

typedef struct {
    int end_select;
    int level;
    PlayingState state;
    Player* player;
    Goal goal;
    int* lines;
    Cars* cars;
} GameData;

typedef union {
    MenuData menu_data; // when GameState is GameMenu or GameSettings or GameSettingsEdit
    GameData game_data; // when GameState is GamePlaying
} GameInfo;

typedef struct {
    Win* top_win;
    Win* main_win;
    Win* bottom_win;
    GameConfig config;
    GameState state;
    GameInfo context_data;
} Game;

void redraw_game_border(const Game*);
Game* create_game();
void run_game_exit(Game*);

#endif //GAME_H
