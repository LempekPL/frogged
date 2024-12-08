#ifndef CAR_H
#define CAR_H
#include "timer.h"
#include "window.h"

typedef enum {
    CarEnemy,
    CarStopping,
    CarFriendly
} CarType;

typedef enum {
    ToLeft,
    ToRight
} CarRide;

typedef struct {
    int x, y, speed;
    CarType car_type;
    CarRide car_ride;
    Timer timer;
} Car;

typedef struct {
    Car* cars;
    int size;
    int capacity;
} Cars;

Car new_car(int x, int y, int move_per_ms, CarType car_type, CarRide car_ride);
Cars* new_cars(int capacity);
void add_car(Cars* cars, const Car* car);
void remove_at_cars(Cars* cars, int index);
void clear_cars(Cars* cars);
void free_cars(const Cars* cars);
void draw_cars(const Win* win, Cars* cars);
void move_cars(Cars* cars, int max);

#endif //CAR_H
