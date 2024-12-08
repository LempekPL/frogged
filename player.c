#include "player.h"
#include <stdlib.h>
#include "util.h"

Player* create_player(int row, int col, char character) {
    Player* player = malloc(sizeof(Player));
    player->y = row;
    player->x = col;
    player->character = character;
    player->curr_pts = 0;
    player->max_pts = 0;
    player->pts = 0;
    return player;
}

void draw_player(const Win* win, const Player* player, const Lines* lines) {
    Line* line = ptr_at_lines(lines, player->y);
    change_color(win, line->type);
    mvwprintw(win->win, player->y, player->x, &player->character);
}

void move_player(Player* player, const int key, const int maxX, const int maxY) {
    switch (key) {
        case 'w':
            if (player->y > 1) {
                player->y--;
                player->curr_pts++;
            } else player->y = 1;
        break;
        case 's':
            if (player->y < maxY) {
                player->y++;
                player->curr_pts--;
            } else player->y = maxY;
        break;
        case 'a':
            if (player->x > 1) player->x--;
            else player->x = 1;
        break;
        case 'd':
            if (player->x < maxX) player->x++;
            else player->x = maxX;
        break;
        default: break;
    }
}