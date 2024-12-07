#ifndef PLAYER_H
#define PLAYER_H
typedef struct {
    int x, y;
    int curr_pts, pts, max_pts;
} Player;

Player* create_player(int row, int col);

#endif //PLAYER_H
