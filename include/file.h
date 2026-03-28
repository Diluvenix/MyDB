#ifndef FILE_H
#define FILE_H
#include <stdio.h>
#include "error.h"

ErrorCode FILE_init(void);
ErrorCode FILE_deinit(void);

ErrorCode FILE_open(int *fd, const char *filename);
ErrorCode FILE_close(int fd);
ErrorCode FILE_seek(int fd, off_t offset, int whence);
ErrorCode FILE_sync(int fd);

ErrorCode FILE_read(int fd, void *buf, size_t n, int *warnings);
ErrorCode FILE_readAt(int fd, off_t position, void *buf, size_t n, int *warnings);
ErrorCode FILE_write(int fd, const void *buf, size_t n);
ErrorCode FILE_writeAt(int fd, off_t position, const void *buf, size_t n);
ErrorCode FILE_truncate(int fd, off_t length);

#endif