#ifndef CAR_H
#define CAR_H
#include "timer.h"
#include "window.h"
#include "lines.h"
#include "player.h"

typedef enum {
    CarEnemy,
    CarStopping,
    CarFriendly
} CarType;

typedef enum {
    ToLeft,
    ToRight
} CarDirection;

typedef struct {
    int x, y;
    int speed;
    float random_speed_change_chance;
    CarType car_type;
    CarDirection car_ride;
    Timer timer;
} Car;

typedef struct CarsStruct {
    Car* cars;
    int size;
    int capacity;
} Cars;

Car new_car(int x, int y, int move_per_ms, float speed_change_chance, CarType car_type, CarDirection car_ride);
void spawn_car_on_line(const Win* win, Cars* cars, const Line* line);
Cars* new_cars(int capacity);
void add_car(Cars* cars, const Car* car);
Car* ptr_at_cars(const Cars* cars, int index);
void remove_at_cars(Cars* cars, int index);
void clear_cars(Cars* cars);
void free_cars(Cars* cars);
void draw_cars(const Win* win, const Cars* cars);
void move_cars(const Win* win, const struct PlayerStruct* player, Cars* cars, int max);

#endif //CAR_H
