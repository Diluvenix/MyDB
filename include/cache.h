#ifndef _CACHE_H_
#define _CACHE_H_

#include <stddef.h>
#include "const.h"
#include "error.h"

ErrorCode CACHE_init();
ErrorCode CACHE_deinit();

ErrorCode CACHE_read(uint64_t tableId, page64_t pagePos, void* buf);
ErrorCode CACHE_devalidate(uint64_t tableId, page64_t pagePos, page64_t n);

#endif