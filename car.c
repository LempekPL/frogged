#include "car.h"
#include <stdlib.h>

void draw_car(const Win* win, const Car car) {
    switch (car.car_type) {
        case CarEnemy:
            wcolor_set(win->win, ROAD_RED_COL, NULL);
            break;
        case CarStopping:
            wcolor_set(win->win, DEFAULT_COL, NULL);
            break;
        case CarFriendly:
            wcolor_set(win->win, ROAD_RED_COL, NULL);
            break;
    }
    mvwprintw(win->win, car.y, car.x, "o=o");
}

void draw_cars(const Win* win, Cars* cars) {
    for (int i = 0; i < cars->size; i++) {
        draw_car(win, cars->cars[i]);
    }
}

int move_car(Car* car, int max) {
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
        car->x += move;
        if (car->x < 1 || max < car->x + 4) {
            return 1;
        }
    }
    return 0;
}

void move_cars(Cars* cars, int max) {
    for (int i = 0; i < cars->size; i++) {
        int outside = move_car(&cars->cars[i], max);
        if (outside) {
            remove_at_cars(cars, i);
        }
    }
}

Car new_car(int x, int y, int move_per_ms, CarType car_type, CarRide car_ride) {
    Timer timer;
    timer_start(&timer);
    Car car = {x, y, move_per_ms, car_type, car_ride, timer};
    return car;
}

Cars* new_cars(int capacity) {
    Cars* newCars = malloc(sizeof(Cars));
    newCars->cars = malloc(sizeof(Car) * capacity);
    newCars->size = 0;
    newCars->capacity = capacity;
    return newCars;
}

void add_car(Cars* cars, const Car* car) {
    if (cars->size == cars->capacity) {
        cars->capacity *= 2;
        cars->cars = realloc(cars->cars, cars->capacity * sizeof(Car));
    }
    cars->cars[cars->size] = *car;
    cars->size++;
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
