#include "config.h"

void save_config(const Game* game) {
    FILE* config_save = fopen("config.txt", "w");
    fprintf(config_save, "border ");
    switch (game->config.border_type) {
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
    fprintf(config_save, "size %d %d\n", game->config.width, game->config.height);
    fprintf(config_save, "seed %lld", game->config.seed);
    fprintf(config_save, "frog %c", game->config.frog);
    fclose(config_save);
}