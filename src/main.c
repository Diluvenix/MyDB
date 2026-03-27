#include <stdio.h>
#include <stdlib.h>
#include "cache.h"
#include "error.h"
#include "logging.h"

int main(void) {
    if (CACHE_init() != ERROR_OK) {
        LOGGING_error("Error whilst initializing cache");
        exit(EXIT_FAILURE);
    }

    printf("Hello, World!\n");

    if (CACHE_deinit() != ERROR_OK) {
        LOGGING_error("Error whilst deinitializing cache");
        exit(EXIT_FAILURE);
    }

    return 0;
}