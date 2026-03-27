#include "cache.h"
#include "error.h"

ErrorCode CACHE_init(){
    return ERROR_OK;
}
ErrorCode CACHE_deinit(){
    return ERROR_OK;
}

ErrorCode CACHE_read(uint64_t filePtr, uint64_t dataId, void* buf, size_t n){
    return ERROR_OK;
}
ErrorCode CACHE_devalidate(uint64_t filePtr, uint64_t dataId, size_t n){
    return ERROR_OK;
}