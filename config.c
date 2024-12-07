#include "config.h"
#include "util.h"
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
    fclose(config_save);
}

GameConfig load_config(char* path_name) {
    GameConfig config = {Simple, 31, 21, time(NULL), 'F'};
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
            char border_type[20];
            fscanf(config_file, "%s", border_type);
            if (strcmp(border_type, "clean") == 0) config.border_type = Clean;
            else if (strcmp(border_type, "wrapped") == 0) config.border_type = Wrapped;
            else config.border_type = Simple;
        } else if (strcmp(name, "size") == 0) {
            int tmpW, tmpH;
            fscanf(config_file, "%d %d", &tmpW, &tmpH);
            config.width = Clamp(tmpW, 31, 1000);
            config.height = Clamp(tmpH, 21, 1000);
        } else if (strcmp(name, "seed") == 0) {
            fscanf(config_file, "%ld", &config.seed);
        } else if (strcmp(name, "frog") == 0) {
            char tmpFrog;
            fscanf(config_file, "%c", &tmpFrog);
            config.frog = tmpFrog;
        }
    }
    fclose(config_file);
    return config;
}
