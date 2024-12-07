#include "game_loop.h"
#include "window.h"
#include "util.h"
#include <stdlib.h>

void game_play_init(Game* game) {
    game->context_data.game_data.player = create_player(game->main_win->rows - 2, game->main_win->cols / 2);
    game->context_data.game_data.lines = calloc(game->main_win->rows, sizeof(int));
    switch (game->context_data.game_data.level) {
        case 1:
            game->context_data.game_data.goal.x = game->main_win->cols / 2;
            game->context_data.game_data.goal.y = 1;
            break;
        case 2:
            game->context_data.game_data.lines[game->main_win->rows / 2] = 1;
            break;
        default: break;
    }

    game->context_data.game_data.state = Playing;
}

void render_lines(Game* game) {
    for (int i = 1; i < game->main_win->rows - 1; i++) {
        for (int j = 1; j < game->main_win->cols - 1; j++) {
            change_color(game->main_win, game->context_data.game_data.lines[i]);
            mvwprintw(game->main_win->win, i, j, " ");
        }
    }
}

void tutorial_text(Game* game) {
    wcolor_set(game->main_win->win, GRASS_BLACK_COL, NULL);
    switch (game->context_data.game_data.level) {
        case 1:
            char* helps1[] = {"^ collect to", "complete level", "you move with WSAD", " or arrow keys", "this is you", "a FROG", "V"};
            mvwprintw(game->main_win->win, 2, game->main_win->cols / 2, helps1[0]);
            mvwprintw(game->main_win->win, 3, game->main_win->cols / 2, helps1[1]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2, centerX(game, helps1[2]), helps1[2]);
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 1, centerX(game, helps1[3]), helps1[3]);
            mvwprintw(game->main_win->win, game->main_win->rows - 5, centerX(game, helps1[4]), helps1[4]);
            mvwprintw(game->main_win->win, game->main_win->rows - 4, centerX(game, helps1[5]), helps1[5]);
            mvwprintw(game->main_win->win, game->main_win->rows - 3, centerX(game, helps1[6]), helps1[6]);
            break;
        case 2:
            char* help1 = "^ this is a road";
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 1, game->main_win->cols - (int) strlen(help1) - 2, help1);
            char* help2 = "you need to";
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 2, game->main_win->cols - (int) strlen(help2) - 2, help2);
            char* help3 = "avoid o=o";
            mvwprintw(game->main_win->win, game->main_win->rows / 2 + 3, game->main_win->cols - (int) strlen(help3) - 2, help3);
            break;
        default: break;
    }
}

void draw_goal(const Game* game) {
    Goal goal = game->context_data.game_data.goal;
    change_color(game->main_win, game->context_data.game_data.lines[goal.y]);
    mvwprintw(game->main_win->win, goal.y, goal.x, "*");
}

void draw_frog(const Game* game) {
    Player* player = game->context_data.game_data.player;
    change_color(game->main_win, game->context_data.game_data.lines[player->y]);
    mvwprintw(game->main_win->win, player->y, player->x, &game->config.frog);
}

void move_frog(Game* game, int key) {
    Player* player = game->context_data.game_data.player;
    switch (key) {
        case 'w':
            if (player->y > 1) {
                player->y--;
                player->curr_pts++;
            } else player->y = 1;
            break;
        case 's':
            if (player->y < game->main_win->rows - 2) {
                player->y++;
                player->curr_pts--;
            } else player->y = game->main_win->rows - 2;
            break;
        case 'a':
            if (player->x > 1) player->x--;
            else player->x = 1;
            break;
        case 'd':
            if (player->x < game->main_win->cols - 2) player->x++;
            else player->x = game->main_win->cols - 2;
            break;
        default: break;
    }
}

void collision_win(Game* game) {
    Goal goal = game->context_data.game_data.goal;
    Player* player = game->context_data.game_data.player;
    if (goal.x == player->x && goal.y == player->y) {
        game->context_data.game_data.state = PlayingSuccess;
        free(player);
    }
}

void game_play_run(Game* game) {
    clear_win(game->top_win);
    mvwprintw(game->top_win->win, 1, 2, "Level: %d", game->context_data.game_data.level);
    render_lines(game);
    tutorial_text(game);
    draw_goal(game);
    draw_frog(game);
    collision_win(game);
    int key = wgetch(game->main_win->win);
    move_frog(game, key);
    switch (key) {
        case 'q':
            game->state = GameMenu;
            game->context_data.menu_data.selected = 0;
            break;
        default: break;
    }
}

void game_play_success(Game* game) {
    clear_win(game->top_win);
    mvwprintw(game->top_win->win, 1, 2, "Level: %d Success", game->context_data.game_data.level);
    wcolor_set(game->main_win->win, DEFAULT_COL, NULL);
    clear_win(game->main_win);
    wrefresh(game->main_win->win);
}

void run_game_play(Game* game) {
    switch (game->context_data.game_data.state) {
        case PlayingInit:
            game_play_init(game);
            break;
        case Playing:
            game_play_run(game);
            break;
        case PlayingSuccess:
            game_play_success(game);
            break;
        default: break;
    }
}