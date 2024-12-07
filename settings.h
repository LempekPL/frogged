#ifndef SETTINGS_H
#define SETTINGS_H

typedef enum {
    Simple,
    Clean,
    Wrapped
} BorderType;

typedef struct {
    BorderType border_type;
    int width, height;
    long long seed;
    char frog;
} GameConfig;

#endif //SETTINGS_H
