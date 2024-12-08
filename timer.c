#include "timer.h"

void timer_start(Timer* timer) {
    timer->start_time = clock();
}

int timer_elapsed(const Timer* timer, int milliseconds) {
    clock_t current_time = clock();
    double elapsed_time_ms = (double)(current_time - timer->start_time) / CLOCKS_PER_SEC * 1000.0;
    return elapsed_time_ms >= milliseconds;
}