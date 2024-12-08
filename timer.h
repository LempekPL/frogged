#ifndef TIMER_H
#define TIMER_H
#include <time.h>

typedef struct {
    time_t start_time;
} Timer;

void timer_start(Timer* timer);
int timer_elapsed(const Timer* timer, int milliseconds);

#endif //TIMER_H
