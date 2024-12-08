#include "config.h"
#include "util.h"
#include "levels.h"
#include <string.h>
#include <time.h>

void save_config(GameConfig* config, char* path_name) {
    FILE* config_save = fopen(path_name, "w");
    fprintf(config_save, "border ");
    switch (config->border_type) {
        case Simple:
            fprintf(config_save, "simple\n");
            break;
        case Clean:
            fprintf(config_save, "clean\n");
            break;
        case Wrapped:
            fprintf(config_save, "wrapped\n");
            break;
    }
    fprintf(config_save, "size %d %d\n", config->width, config->height);
    fprintf(config_save, "seed %lld\n", config->seed);
    fprintf(config_save, "frog %c\n", config->frog);
    fprintf(config_save, "cooldown %d\n", config->cooldown);
    fprintf(config_save, "completed %d\n", config->completed);
    fclose(config_save);
}

void border_config(FILE* config_file, GameConfig* config) {
    char border_type[20];
    fscanf(config_file, "%s", border_type);
    if (strcmp(border_type, "clean") == 0) config->border_type = Clean;
    else if (strcmp(border_type, "wrapped") == 0) config->border_type = Wrapped;
    else config->border_type = Simple;
}

void size_config(FILE* config_file, GameConfig* config) {
    int tmpW, tmpH;
    fscanf(config_file, "%d %d", &tmpW, &tmpH);
    config->width = Clamp(tmpW, 31, 1000);
    config->height = Clamp(tmpH, 21, 1000);
}

GameConfig load_config(char* path_name) {
    GameConfig config = {Simple, 31, 21, time(NULL), 'F', 200, 1};
    FILE* config_file = fopen(path_name, "r");
    if (config_file == NULL) {
        fclose(config_file);
        save_config(&config, path_name);
        return config;
    }
    while (!feof(config_file)) {
        char name[20];
        fscanf(config_file, "%s ", name);
        if (strcmp(name, "border") == 0) {
            border_config(config_file, &config);
        } else if (strcmp(name, "size") == 0) {
            size_config(config_file, &config);
        } else if (strcmp(name, "seed") == 0) {
            fscanf(config_file, "%ld", &config.seed);
        } else if (strcmp(name, "frog") == 0) {
            fscanf(config_file, "%c", &config.frog);
        } else if (strcmp(name, "cooldown") == 0) {
            int tmp_cooldown;
            fscanf(config_file, "%d", &tmp_cooldown);
            config.cooldown = Clamp(tmp_cooldown, 0, 10000);
        } else if (strcmp(name, "completed") == 0) {
            int tmp_completed;
            fscanf(config_file, "%d", &tmp_completed);
            config.completed = Clamp(tmp_completed, 1, MAX_LEVELS);
        }
    }
    fclose(config_file);
    return config;
}
