#include "lines.h"
#include <stdlib.h>
#include "util.h"

void draw_line(const Win* win, const Line line, const int splitter) {
    change_color(win, line.type);
    for (int i = 1; i < win->cols - 1; i++) {
        mvwprintw(win->win, line.y, i, splitter == 1 && i % 2 == 0 ? "_" : " ");
    }
}

void draw_lines(const Win* win, Lines* lines) {
    for (int i = 1; i < win->rows - 1; i++) {
        int splitter = 0;
        if (i < lines->size && lines->lines[i+1].type == LineRoad && lines->lines[i].type == LineRoad) {
            splitter = 1;
        }
        draw_line(win, lines->lines[i], splitter);
    }
}

Line new_line(LineType type, int y) {
    Line line = {type, y};
    if (type == LineRoad) {
        Timer timer;
        timer_start(&timer);
        line.line_data.car.spawn_timer = timer;
        line.line_data.car.next_car = RA(0, 100);
        line.line_data.car.min_next_car = 800;
        line.line_data.car.max_next_car = 1200;
        line.line_data.car.line_speed_limit = 200;
    }
    return line;
}

Line new_line_car(LineType type, int y, LineDirection direction, int speed_limit, int next_car_in, int min_next_car, int max_next_car) {
    Line line = {type, y};
    Timer timer;
    timer_start(&timer);
    line.line_data.car.spawn_timer = timer;
    line.line_data.car.next_car = next_car_in;
    line.line_data.car.cars_direction = direction;
    line.line_data.car.min_next_car = min_next_car;
    line.line_data.car.max_next_car = max_next_car;
    line.line_data.car.line_speed_limit = speed_limit;
    line.line_data.car.stopper_chance = 0;
    return line;
}

Lines* new_lines(int capacity) {
    Lines* new_line_vec = malloc(sizeof(Lines));
    new_line_vec->lines = malloc(sizeof(Line) * capacity);
    new_line_vec->size = 0;
    new_line_vec->capacity = capacity;
    return new_line_vec;
}

void add_line(Lines* lines, const Line* line) {
    if (lines->size == lines->capacity) {
        lines->capacity *= 2;
        lines->lines = realloc(lines->lines, lines->capacity * sizeof(Line));
    }
    lines->lines[lines->size] = *line;
    lines->size++;
}

void shift_at_lines(Lines* lines, const Line* line, int index) {
    if (lines->size+1 >= lines->capacity) {
        lines->capacity *= 2;
        lines->lines = realloc(lines->lines, lines->capacity * sizeof(Line));
    }
    if (index < 0 || index >= lines->size) {
        exit(EXIT_FAILURE);
    }
    for (int i = index; i < lines->size - 1; i++) {
        lines->lines[i+1] = lines->lines[i];
    }
    lines->lines[index] = *line;
    lines->size++;
}

Lines* generate_default_lines(int height) {
    Lines* new_lines_vec = new_lines(height);
    for (int i = 0; i < height; i++) {
        Line line = new_line(LineGrass, i);
        add_line(new_lines_vec, &line);
    }
    return new_lines_vec;
}

Line* ptr_at_lines(const Lines* lines, const int index) {
    if (index < 0 || index >= lines->size) {
        exit(EXIT_FAILURE);
    }
    return &lines->lines[index];
}

void replace_self_lines(const Lines* lines, const Line* line) {
    int index = line->y;
    if (index < 0 || index >= lines->size) {
        exit(EXIT_FAILURE);
    }
    lines->lines[index] = *line;
}

void replace_at_lines(const Lines* lines, const Line* line, int index) {
    if (index < 0 || index >= lines->size) {
        exit(EXIT_FAILURE);
    }
    lines->lines[index] = *line;
}

void remove_at_lines(Lines* lines, int index) {
    if (index < 0 || index >= lines->size) {
        exit(EXIT_FAILURE);
    }
    for (int i = index; i < lines->size - 1; i++) {
        lines->lines[i] = lines->lines[i + 1];
    }
    lines->size--;
}

void clear_lines(Lines* lines) {
    free(lines->lines);
    lines->lines = malloc(sizeof(Line) * lines->capacity);
    lines->size = 0;
}

void free_lines(Lines* lines) {
    free(lines->lines);
    free(lines);
}
