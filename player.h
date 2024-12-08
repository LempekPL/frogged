#ifndef PLAYER_H
#define PLAYER_H
struct GameDataS;
typedef struct GameDataS GameData;
#include "game.h"

typedef struct PlayerS {
    int x, y;
    int curr_pts, pts, max_pts;
    char character;
    Timer timer;
    int cooldown;
} Player;

typedef enum {
    PlayerTouchNothing,
    PlayerTouchGoal,
    PlayerTouchDeath,
} PlayerCollision;

Player* create_player(int row, int col, char character, int cooldown);
PlayerCollision collision_player(const GameData* game_data);
void draw_player(const Win* win, const Player* player, const Lines* lines);
void move_player(Player* player, int key, int maxX, int maxY);

#endif //PLAYER_H
