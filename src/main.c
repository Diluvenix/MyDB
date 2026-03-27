#include <stdio.h>
#include <stdlib.h>
#include "cache.h"
#include "error.h"
#include "file.h"
#include "logging.h"

int main(void) {
    if (FILE_init() != ERROR_OK) {
        LOGGING_error("Error whilst initializing filesystem");
        exit(EXIT_FAILURE);
    }

    if (CACHE_init() != ERROR_OK) {
        LOGGING_error("Error whilst initializing cache");
        FILE_deinit();
        exit(EXIT_FAILURE);
    }

    printf("Hello, World!\n");

    if (CACHE_deinit() != ERROR_OK) {
        LOGGING_error("Error whilst deinitializing cache");
        FILE_deinit();
        exit(EXIT_FAILURE);
    }

    if (FILE_deinit() != ERROR_OK) {
        LOGGING_error("Error whilst deinitializing filesystem");
        exit(EXIT_FAILURE);
    }

    return 0;
}