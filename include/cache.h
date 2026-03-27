#ifndef _CACHE_H_
#define _CACHE_H_

#include <stddef.h>
#include <stdint.h>
#include "error.h"

ErrorCode CACHE_init();
ErrorCode CACHE_deinit();

ErrorCode CACHE_read(uint64_t fileId, uint64_t dataId, void* buf, size_t n);
ErrorCode CACHE_write(uint64_t fileId, uint64_t dataId, const void* buf, size_t n);

#endif