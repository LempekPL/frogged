#include "lines.h"
#include <stdlib.h>
#include "util.h"

void draw_line(const Win* win, const Line line) {
    change_color(win, line.type);
    for (int i = 1; i < win->cols - 1; i++) {
        mvwprintw(win->win, line.y, i, " ");
    }
}

void draw_lines(const Win* win, Lines* lines) {
    for (int i = 1; i < win->rows - 1; i++) {
        draw_line(win, lines->lines[i]);
    }
}

Line new_line(LineType type, int y) {
    Timer timer;
    timer_start(&timer);
    Line line = {type, y, ToRight, -1, 0, 0,  timer, 0, 0};
    if (type == LineRoad) {
        line.next_car = RA(0, 100);
        line.min_random = 800;
        line.max_random = 1200;
        line.line_speed_limit = 200;
    }
    return line;
}

Line new_line_ext(LineType type, int y, LineDirection direction, int speed_limit, int car_freq, int min_random, int max_random) {
    Timer timer;
    timer_start(&timer);
    Line line = {type, y, direction, car_freq, min_random, max_random, timer, speed_limit, 0};
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
        lines->lines = realloc(lines->lines, lines->capacity * sizeof(Lines));
    }
    lines->lines[lines->size] = *line;
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
