#ifndef PLAYER_H
#define PLAYER_H
#include "window.h"
#include "lines.h"

typedef struct {
    int x, y;
    int curr_pts, pts, max_pts;
    char character;
} Player;

Player* create_player(int row, int col, char character);
void draw_player(const Win* win, const Player* player, const Lines* lines);
void move_player(Player* player, int key, int maxX, int maxY);

#endif //PLAYER_H
