#include "car.h"
#include <stdlib.h>

void draw_car(const Win* win, const Car car) {
    switch (car.car_type) {
        case CarEnemy:
            wcolor_set(win->win, ROAD_RED_COL, NULL);
            break;
        case CarStopping:
            wcolor_set(win->win, ROAD_YELLOW_COL, NULL);
            break;
        case CarFriendly:
            wcolor_set(win->win, ROAD_RED_COL, NULL);
            break;
    }
    mvwprintw(win->win, car.y, car.x, "o=o");
}

void draw_cars(const Win* win, const Cars* cars) {
    for (int i = 0; i < cars->size; i++) {
        draw_car(win, cars->cars[i]);
    }
}

int move_car(const Win* win, const Player* player, Car* car, int max) {
    if (timer_elapsed(&car->timer, car->speed)) {
        timer_start(&car->timer);
        int move = 0;
        switch (car->car_ride) {
            case ToLeft:
                move = -1;
                break;
            case ToRight:
                move = 1;
                break;
        }
        int ch = mvwinch(win->win, car->y, car->x+move) & A_CHARTEXT;
        int chAfter = mvwinch(win->win, car->y, car->x+move*2) & A_CHARTEXT;
        if (ch != 'o' && chAfter != '=') {
            if (car->car_type != CarStopping || ch != player->character) {
                car->x += move;
            }
        }
        if (car->x < 1 || max < car->x + 4) {
            return 1;
        }
    }
    return 0;
}

void move_cars(const Win* win, const Player* player, Cars* cars, int max) {
    for (int i = 0; i < cars->size; i++) {
        int outside = move_car(win, player, &cars->cars[i], max);
        if (outside) {
            remove_at_cars(cars, i);
        }
    }
}

Car new_car(int x, int y, int move_per_ms, CarType car_type, CarDirection car_ride) {
    Timer timer;
    timer_start(&timer);
    Car car = {x, y, move_per_ms, car_type, car_ride, timer};
    return car;
}

Car spawn_car_on_line(const Win* win, Line line, int move_per_ms, CarType car_type) {
    Timer timer;
    timer_start(&timer);
    Car car = {1, line.y, move_per_ms, car_type, line.cars_direction, timer};
    if (line.cars_direction == ToLeft) {
        car.x = win->cols - 4;
    }
    return car;
}

Cars* new_cars(int capacity) {
    Cars* new_car_vec = malloc(sizeof(Cars));
    new_car_vec->cars = malloc(sizeof(Car) * capacity);
    new_car_vec->size = 0;
    new_car_vec->capacity = capacity;
    return new_car_vec;
}

void add_car(Cars* cars, const Car* car) {
    if (cars->size == cars->capacity) {
        cars->capacity *= 2;
        cars->cars = realloc(cars->cars, cars->capacity * sizeof(Car));
    }
    cars->cars[cars->size] = *car;
    cars->size++;
}

Car* ptr_at_cars(const Cars* cars, const int index) {
    if (index < 0 || index >= cars->size) {
        exit(EXIT_FAILURE);
    }
    return &cars->cars[index];
}

void remove_at_cars(Cars* cars, int index) {
    if (index < 0 || index >= cars->size) {
        exit(EXIT_FAILURE);
    }
    for (int i = index; i < cars->size - 1; i++) {
        cars->cars[i] = cars->cars[i + 1];
    }
    cars->size--;
}

void clear_cars(Cars* cars) {
    free(cars->cars);
    cars->cars = malloc(sizeof(Car) * cars->capacity);
    cars->size = 0;
}

void free_cars(Cars* cars) {
    free(cars->cars);
    free(cars);
}
