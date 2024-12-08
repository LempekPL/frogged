#include "window.h"
#include "game.h"
#include "game_loop.h"
#include "menus.h"

// void calc_pts(Game* game) {
//     if (game->player->curr_pts > game->player->pts) {
//         game->player->pts = game->player->curr_pts;
//     }
//     if (game->player->pts > game->player->max_pts) {
//         game->player->max_pts = game->player->pts;
//     }
// }

///////////////////////////////////////////////////
/// TODO LIST:
/// - config size
/// - trees/logs
/// - cars but friendly
/// - ranking (infinite)
/// - stork

int main() {
    init_ncurses();
    Game* game = create_game();
    while (1) {
        clear_win(game->main_win);
        switch (game->state) {
            case GameMenu:
                run_game_main_menu(game);
                break;
            case GameMenuStart:
                run_game_start_menu(game);
                break;
            case GameMenuLevels:
                run_game_levels_menu(game);
                break;
            case GameHelp:
                run_game_help(game);
                break;
            case GameSettings:
                run_game_settings_menu(game);
                break;
            case GameSettingsEdit:
                run_game_settings_edit(game);
                break;
            case GamePlaying:
                run_game_play(game);
                break;
            case GameExit:
                run_game_exit(game);
                return 0;
        }
    }
}
