#include "cache.h"
#include "error.h"
#include "logging.h"
#include "table_head.h"

static TableHead systemTable = {
    0, "_system",
    0, 1, -1, -1
};

ErrorCode CACHE_init(){
    TRY(TableHead_reopen(&systemTable), "Error whilst loading table head for table \"_system\"");
    
    LOGGING_info("Sucessfully initialized cache");
    return ERROR_OK;
}
ErrorCode CACHE_deinit(){
    TRY(TableHead_close(&systemTable), "Error whilst closing table head for table \"_system\"");

    LOGGING_info("Sucessfully deinitialized cache");
    return ERROR_OK;
}

ErrorCode CACHE_read(uint64_t tableId, page64_t pagePos, void* buf){
    if (tableId == 0 && pagePos == 0) {
        buf = &systemTable;
        return ERROR_OK;
    }

    return ERROR_NOT_IMPLEMENTED;
}
ErrorCode CACHE_devalidate(uint64_t tableId, page64_t pagePos){
    if (tableId == 0 && pagePos == 0)
        return ERROR_OK;

    return ERROR_NOT_IMPLEMENTED;
}