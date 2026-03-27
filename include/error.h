#ifndef _ERROR_H_
#define _ERROR_H_

typedef enum e_ErrorCode {
    ERROR_OK = 0,
    ERROR_FILE_IO,
    ERROR_NOT_IMPLEMENTED,
} ErrorCode;

#define WARNING_EOF (1 << 0)

#endif