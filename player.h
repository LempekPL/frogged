#ifndef PLAYER_H
#define PLAYER_H
typedef struct GameDataStruct GameData;
#include "game.h"

typedef struct PlayerStruct {
    int x, y;
    int curr_pts, pts, max_pts;
    char character;
    Timer timer;
    int cooldown;
    int is_riding;
} Player;

typedef enum {
    PlayerTouchNothing,
    PlayerTouchGoal,
    PlayerTouchDeath,
} PlayerCollision;

Player* create_player(int row, int col, char character, int cooldown);
PlayerCollision collision_player(const GameData* game_data);
void draw_player(const Win* win, const Player* player, const struct LinesStruct* lines);
void move_player(Player* player, int key, int maxX, int maxY);

#endif //PLAYER_H
