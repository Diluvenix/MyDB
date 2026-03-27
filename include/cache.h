#ifndef _CACHE_H_
#define _CACHE_H_

#include <stddef.h>
#include <stdint.h>
#include "error.h"

ErrorCode CACHE_init();
ErrorCode CACHE_deinit();

ErrorCode CACHE_read(uint64_t filePtr, uint64_t dataId, void* buf, size_t n);
ErrorCode CACHE_devalidate(uint64_t filePtr, uint64_t dataId, size_t n);

#endif