#include "cache.h"
#include "error.h"
#include "table_head.h"

static TableHead systemTable = {
    0, "_system",
    0, 1, -1, -1
};

ErrorCode CACHE_init(){
    return ERROR_OK;
}
ErrorCode CACHE_deinit(){
    return ERROR_OK;
}

ErrorCode CACHE_read(uint64_t tableId, page64_t pagePos, void* buf){
    if (tableId == 0 && pagePos == 0) {
        buf = &systemTable;
        return ERROR_OK;
    }

    return ERROR_NOT_IMPLEMENTED;
}
ErrorCode CACHE_devalidate(uint64_t tableId, page64_t pagePos, page64_t n){
    return ERROR_OK;
}