#include "cache.h"
#include "error.h"

ErrorCode CACHE_init(){
    return ERROR_OK;
}
ErrorCode CACHE_deinit(){
    return ERROR_OK;
}

ErrorCode CACHE_read(uint64_t fileId, uint64_t dataId, void* buf, size_t n){
    return ERROR_OK;
}
ErrorCode CACHE_write(uint64_t fileId, uint64_t dataId, const void* buf, size_t n){
    return ERROR_OK;
}