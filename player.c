#include "player.h"
#include <stdlib.h>

Player* create_player(int row, int col) {
    Player* player = malloc(sizeof(Player));
    player->y = row;
    player->x = col;
    player->curr_pts = 0;
    player->max_pts = 0;
    player->pts = 0;
    return player;
}
