#include "player.h"
#include "game_loop.h"
#include "util.h"
#include <stdlib.h>

Player* create_player(const int row, const int col, const char character, const int cooldown) {
    Timer timer;
    timer_start(&timer);
    Player* player = malloc(sizeof(Player));
    player->y = row;
    player->x = col;
    player->character = character;
    player->timer = timer;
    player->cooldown = cooldown;
    player->curr_pts = 0;
    player->max_pts = 0;
    player->pts = 0;
    player->is_riding = 0;
    return player;
}

int is_on_log(Player* player, LineWaterData* line_data) {
    for (int i = 0; i < line_data->logs_amount; i++) {
        if (player->x == line_data->logs[i]) {
            return 1;
        }
    }
    return 0;
}

PlayerCollision collision_player(const GameData* game_data) {
    Goal goal = game_data->goal;
    Player* player = game_data->player;
    Line* line = &game_data->lines->lines[player->y];
    if (goal.x == player->x && goal.y == player->y) {
        return PlayerTouchGoal;
    }
    for (int i = 0; i < game_data->cars->size; i++) {
        Car* car = ptr_at_cars(game_data->cars, i);
        if (car->y == player->y && car->x <= player->x && car->x + 3 > player->x && car->car_type != CarFriendly) {
            return PlayerTouchDeath;
        }
    }
    if (line->type == LineWater) {
        LineWaterData line_data = line->line_data.water;
        if (!is_on_log(player, &line_data)) {
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
    int dx = 0, dy = 0;
    switch (key) {
        case 'w':
        case KEY_UP:
            dy = -1;
            break;
        case 's':
        case KEY_DOWN:
            dy = 1;
            break;
        case 'a':
        case KEY_LEFT:
            dx = -1;
            break;
        case 'd':
        case KEY_RIGHT:
            dx = 1;
        break;
        default: return;
    }
    if (timer_elapsed(&player->timer, player->cooldown)) {
        timer_start(&player->timer);
        if (dx != 0 && player->x + dx >= 1 && player->x + dx <= maxX) {
            player->x += dx;
        }
        if (dy != 0 && player->y + dy >= 1 && player->y + dy <= maxY) {
            player->y += dy;
            player->curr_pts += dy == 1 ? -1 : 1;
        }
    }
}
