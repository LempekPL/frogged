#ifndef CONFIG_H
#define CONFIG_H

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
    int cooldown;
} GameConfig;

void save_config(GameConfig*, char* path_name);
GameConfig load_config(char* path_name);

#endif //CONFIG_H
