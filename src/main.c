#include <stdlib.h>
#include "cache.h"
#include "error.h"
#include "file.h"
#include "logging.h"
#include "table_head.h"

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

    TableHead *_system;
    if (CACHE_read(0, 0, (void **)&_system) != ERROR_OK) {
        LOGGING_error("Error whilst reading \"_system\" database");
        goto exit_error;
    }
    if (TableHead_insertKeyValue(_system, 5, 25, NULL) != ERROR_OK) {
        LOGGING_error("Error whilst inserting [5]:[25] into table \"_system\"");
        goto exit_error;
    }


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

exit_error:
    CACHE_deinit();
    FILE_deinit();
    exit(EXIT_FAILURE);
}