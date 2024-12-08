#include "player.h"
#include <stdlib.h>

#include "game_loop.h"
#include "util.h"

Player* create_player(const int row, const int col, const char character) {
    Player* player = malloc(sizeof(Player));
    player->y = row;
    player->x = col;
    player->character = character;
    player->curr_pts = 0;
    player->max_pts = 0;
    player->pts = 0;
    return player;
}

PlayerCollision collision_player(const GameData* game_data) {
    Goal goal = game_data->goal;
    Player* player = game_data->player;
    if (goal.x == player->x && goal.y == player->y) {
        return PlayerTouchGoal;
    }
    for (int i = 0; i < game_data->cars->size; i++) {
        Car* car = ptr_at_cars(game_data->cars, i);
        if (car->y == player->y && car->x <= player->x && car->x + 3 > player->x) {
            return PlayerTouchDeath;
        }
    }
    return PlayerTouchNothing;
}

void draw_player(const Win* win, const Player* player, const Lines* lines) {
    Line* line = ptr_at_lines(lines, player->y);
    change_color(win, line->type);
    mvwprintw(win->win, player->y, player->x, "%c", player->character);
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