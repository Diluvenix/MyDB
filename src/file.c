#include "file.h"

#include "error.h"
#include "logging.h"
#include <asm-generic/errno-base.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

static int dir = -1;

ErrorCode FILE_init(void) {
    assert(dir < 0);

    while (1)
    {
        if ((dir = open("./data", O_RDONLY | O_DIRECTORY)) >= 0) break;
        if (errno == EINTR) continue;
        else if (errno == ENOENT) {
            if (mkdir("./data", S_IRWXU) != 0) {
                LOGGING_perror("Error whilst creating data directory");
                return ERROR_FILE_IO;
            }
        }
        else {
            LOGGING_perror("Error whilst opening data directory");
            return ERROR_FILE_IO;
        }
    }

    LOGGING_info("Sucessfully initialized data directory");
    return ERROR_OK;
}

ErrorCode FILE_deinit(void) {
    assert(dir >= 0);
    
    while (fsync(dir)) {
        if (errno != EINTR) {
            LOGGING_perror("Error whilst syncing directory to disk");
            return ERROR_FILE_IO;
        }
    }
    while (close(dir)) {
        if (errno != EINTR) {
            LOGGING_perror("Error whilst closing directory");
            return ERROR_FILE_IO;
        }
    }

    dir = -1;
    LOGGING_info("Sucessfully deinitialized data directory");
    return ERROR_OK;
}

ErrorCode FILE_open(int *fd, const char *filename) {
    assert(dir >= 0);

    while ((*fd = openat(dir, filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
        if (errno != EINTR) {
            LOGGING_perror("Error whilst opening file %s", filename);
            return ERROR_FILE_IO;
        }
    }

    LOGGING_debug("Sucessfully opened file \"%s\" as %d", filename, *fd);
    return ERROR_OK;
}
ErrorCode FILE_close(int fd) {
    while (close(fd) < 0) {
        if (errno != EINTR) {
            LOGGING_perror("Error whilst closing file descriptor %d", fd);
            return ERROR_FILE_IO;
        }
    }

    LOGGING_debug("Sucessfully closed file %d", fd);
    return ERROR_OK;
}

ErrorCode FILE_seek(int fd, off_t offset, int whence) {
    if (lseek(fd, offset, whence) == ((off_t)-1)) {
        LOGGING_perror("Error whilst seeking position in file %d", fd);
        return ERROR_FILE_IO;
    }

    LOGGING_debug("Sucessfully seeked position in file %d", fd);
    return ERROR_OK;
}

ErrorCode FILE_sync(int fd) {
    assert(dir >= 0);

    while (fsync(fd)) {
        if (errno != EINTR)
        {
            LOGGING_perror("Error whilst syncing file %d to disk", fd);
            return ERROR_FILE_IO;
        }
    }

    while (fsync(dir)) {
        if (errno != EINTR)
        {
            LOGGING_perror("Error whilst syncing data directory to disk");
            return ERROR_FILE_IO;
        }
    }

    LOGGING_debug("Sucessfully synced file %d to disk", fd);
    return ERROR_OK;
}

ErrorCode FILE_read(int fd, void *buf, size_t n, int *warnings) {
    size_t offset = 0;
    ssize_t nread;

    while (offset < n) {
        if ((nread = read(fd, buf + offset, n - offset)) > 0) {
            offset += nread;
        }
        else if (nread == 0) {
            LOGGING_warning("Encountered EOF whilst reading data from file %d", fd);
            *warnings |= WARNING_EOF;
            return ERROR_FILE_IO;
        }
        else if (errno != EINTR) {
            LOGGING_perror("Error whilst reading data from file %d", fd);
            return ERROR_FILE_IO;
        }
    }

    LOGGING_debug("Sucessfully read %zu bytes from file %d", n, fd);
    return ERROR_OK;
}

ErrorCode FILE_readAt(int fd, off_t position, void *buf, size_t n, int *warnings) {
    size_t offset = 0;
    ssize_t nread;

    while (offset < n) {
        if ((nread = pread64(fd, buf + offset, n - offset, position + offset)) > 0) {
            offset += nread;
        }
        else if (nread == 0) {
            LOGGING_warning("Encountered EOF whilst reading data from file %d", fd);
            *warnings |= WARNING_EOF;
            return ERROR_FILE_IO;
        }
        else if (errno != EINTR) {
            LOGGING_perror("Error whilst reading data from file %d", fd);
            return ERROR_FILE_IO;
        }
    }

    LOGGING_debug("Sucessfully read %zu bytes from file %d", n, fd);
    return ERROR_OK;
}

ErrorCode FILE_write(int fd, const void *buf, size_t n) {
    size_t offset = 0;
    ssize_t nwrite;

    while (offset < n) {
        if ((nwrite = write(fd, buf + offset, n - offset)) > 0) {
            offset += nwrite;
        }
        else if (errno != EINTR) {
            LOGGING_perror("Error whilst writing data into file %d", fd);
            return ERROR_FILE_IO;
        }
    }

    LOGGING_debug("Sucessfully wrote %zu bytes into file %d", n, fd);
    return ERROR_OK;
}

ErrorCode FILE_writeAt(int fd, off_t position, const void *buf, size_t n) {
    size_t offset = 0;
    ssize_t nwrite;

    while (offset < n) {
        if ((nwrite = pwrite64(fd, buf + offset, n - offset, position + offset)) > 0) {
            offset += nwrite;
        }
        else if (errno != EINTR) {
            LOGGING_perror("Error whilst writing data into file %d", fd);
            return ERROR_FILE_IO;
        }
    }

    LOGGING_debug("Sucessfully wrote %zu bytes into file %d", n, fd);
    return ERROR_OK;
}

ErrorCode FILE_truncate(int fd, off_t length) {
    if (ftruncate(fd, length)) {
        LOGGING_perror("Error whilst truncating file %d to %zu bytes", fd, length);
        return ERROR_FILE_IO;
    }

    LOGGING_debug("Sucessfully truncated file %d to %zu bytes", fd, length);
    return ERROR_OK;
}
