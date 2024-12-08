#ifndef GAME_LINES_H
#define GAME_LINES_H
#include "timer.h"
#include "window.h"

typedef enum {
    LineGrass,
    LineRoad,
    LineWater,
} LineType;

typedef enum {
    DirToLeft,
    DirToRight
} LineDirection;

typedef struct {
    LineType type;
    int y;
    LineDirection cars_direction;
    int next_car, min_random, max_random;
    Timer spawn_timer;
    int line_speed_limit;
    float stopper_chance; // 1 - 100%, 0 - 0%
} Line;

typedef struct {
    Line* lines;
    int size;
    int capacity;
} Lines;

Line new_line(LineType type, int y);
Line new_line_ext(LineType type, int y, LineDirection direction, int speed_limit, int car_freq, int min_random, int max_random);
Lines* generate_default_lines(int height);
Lines* new_lines(int capacity);
void add_line(Lines* lines, const Line* line);
Line* ptr_at_lines(const Lines* lines, int index);
void replace_at_lines(const Lines* lines, const Line* line, int index);
void remove_at_lines(Lines* lines, int index);
void clear_lines(Lines* lines);
void free_lines(Lines* lines);
void draw_lines(const Win* win, Lines* lines);

#endif //GAME_LINES_H
