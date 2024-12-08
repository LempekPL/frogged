#include "car.h"
#include <stdlib.h>

#include "util.h"

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

int is_next_empty(const Win* win, const struct PlayerStruct* player, Car* car) {
    int move = 0;
    int car_size = 0;
    switch (car->car_ride) {
        case ToLeft:
            move = -1;
        break;
        case ToRight:
            move = 1;
            car_size = 2;
        break;
    }
    int next_char = mvwinch(win->win, car->y, car->x+move+car_size) & A_CHARTEXT;
    int next_char_after = mvwinch(win->win, car->y, car->x+move*2+car_size) & A_CHARTEXT;
    if (!(next_char == 'o' && next_char_after == '=') && (car->car_type != CarStopping || next_char != player->character)) {
        return 1;
    }
    return 0;
}

int move_car(const Win* win, const struct PlayerStruct* player, Car* car, int max) {
    if (timer_elapsed(&car->timer, car->speed)) {
        float chance = (float) rand() / (float) RAND_MAX;
        if (chance <= car->random_speed_change_chance) {
            car->speed = Clamp(car->speed+ RA(-100, 100), 100, 1000);
        }
        int move = 0;
        switch (car->car_ride) {
            case ToLeft:
                move = -1;
                break;
            case ToRight:
                move = 1;
                break;
        }
        if (is_next_empty(win, player, car)) {
            car->x += move;
        }
        timer_start(&car->timer);
        if (car->x < 1 || max < car->x + 4) {
            return 1;
        }
    }
    return 0;
}

void move_cars(const Win* win, const struct PlayerStruct* player, Cars* cars, int max) {
    for (int i = 0; i < cars->size; i++) {
        int outside = move_car(win, player, &cars->cars[i], max);
        if (outside) {
            remove_at_cars(cars, i);
        }
    }
}

Car new_car(int x, int y, int move_per_ms, float speed_change_chance, CarType car_type, CarDirection car_ride) {
    Timer timer;
    timer_start(&timer);
    Car car = {x, y, move_per_ms, speed_change_chance, car_type, car_ride, timer};
    return car;
}

int is_area_clear(const Win* win, const int y, const int x) {
    chtype buff[6];
    mvwinchnstr(win->win, y, x - 1, buff, 5);
    buff[5] = '\0';
    char car_area[6];
    for (int i = 0; i < 5; i++) {
        car_area[i] = (char)(buff[i] & A_CHARTEXT);
    }
    car_area[5] = '\0';
    for (int i = 0; i < 5; i++) {
        if ((car_area[i] == '=' && car_area[i + 1] == 'o') || (car_area[i] == 'o' && car_area[i + 1] == '=')) {
            return 0;
        }
    }
    return 1;
}

void spawn_car_on_line(const Win* win, Cars* cars, const Line* line) {
    float speed_change_chance = (float) rand() / (float) RAND_MAX;
    Timer timer;
    timer_start(&timer);
    const LineCarData* line_data = &line->line_data.car;
    Car car = {1, line->y, line_data->line_speed_limit, speed_change_chance, CarEnemy, line_data->cars_direction, timer};
    if (line_data->cars_direction == ToLeft) {
        car.x = win->cols - 4;
    }
    float chance = (float) rand() / (float) RAND_MAX;
    if (chance <= line_data->stopper_chance) {
        car.car_type = CarStopping;
    }
    if (is_area_clear(win, car.y, car.x)) {
        add_car(cars, &car);
    }
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
