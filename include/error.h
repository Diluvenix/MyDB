#ifndef _ERROR_H_
#define _ERROR_H_

#include "logging.h"

typedef enum e_ErrorCode {
    ERROR_OK = 0,
    ERROR_FILE_IO,
    ERROR_JOURNAL_UNSTAGED,
    ERROR_TABLE_INSERTION,
    ERROR_NOT_IMPLEMENTED,
} ErrorCode;

#define TRY(func, format, ...) do {ErrorCode e = func; if(e != ERROR_OK) { LOGGING_error(format __VA_OPT__(,) __VA_ARGS__); return e; }} while (0);

#define WARNING_NONE            0
#define WARNING_EOF             (1 << 0)
#define WARNING_KEY_NOT_UNIQUE  (1 << 1)

#endif