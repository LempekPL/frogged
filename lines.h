#ifndef GAME_LINES_H
#define GAME_LINES_H
#include "timer.h"
#include "window.h"

typedef enum {
    DirToLeft,
    DirToRight,
} LineDirection;

typedef struct {
    LineDirection cars_direction;
    int next_car, min_next_car, max_next_car;
    Timer spawn_timer;
    int line_speed_limit;
    float stopper_chance; // 1 - 100%, 0 - 0%
} LineCarData;

typedef struct {

} LineGrassData;

typedef struct {
    int* logs;
    int logs_amount;
} LineWaterData;

typedef union {
    LineCarData car;
    LineGrassData grass;
    LineWaterData water;
} LineData;

typedef enum {
    LineGrass,
    LineRoad,
    LineWater,
} LineType;

typedef struct LineStruct {
    LineType type;
    int y;
    LineData line_data;
} Line;

typedef struct LinesStruct {
    Line* lines;
    int size;
    int capacity;
} Lines;

Line new_line(LineType type, int y);
Line new_line_car(LineType type, int y, LineDirection direction, int speed_limit, int next_car_in, int min_next_car, int max_next_car);
Lines* generate_default_lines(int height);
Lines* new_lines(int capacity);
void add_line(Lines* lines, const Line* line);
Line* ptr_at_lines(const Lines* lines, int index);
void replace_self_lines(const Lines* lines, const Line* line);
void replace_at_lines(const Lines* lines, const Line* line, int index);
void remove_at_lines(Lines* lines, int index);
void clear_lines(Lines* lines);
void free_lines(Lines* lines);
void draw_lines(const Win* win, Lines* lines);

#endif //GAME_LINES_H
