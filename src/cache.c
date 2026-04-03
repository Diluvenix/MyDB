#include "cache.h"

#include <stdlib.h>
#include "const.h"
#include "error.h"
#include "file.h"
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

ErrorCode CACHE_read(uint64_t tableId, page64_t pagePos, void **buf){
    if (tableId == 0 && pagePos == 0) {
        *buf = &systemTable;
        return ERROR_OK;
    } 
    else if (tableId == 0) {
        void *cacheRef = malloc(PAGE_SIZE);
        if (cacheRef == NULL) {
            LOGGING_perror("Error whilst allocating memory for new node!");
            return ERROR_NOT_IMPLEMENTED;
        }

        TRY(FILE_readAt(systemTable.filePtr, pagePos << PAGE_POW, cacheRef, PAGE_SIZE, NULL), "Error whilst reading from position %" PRIp64 " in table [%" PRIu64 "]", pagePos, tableId);

        *buf = cacheRef;
        return ERROR_OK;
    }

    return ERROR_NOT_IMPLEMENTED;
}
ErrorCode CACHE_devalidate(uint64_t tableId, page64_t pagePos){
    if (tableId == 0 && pagePos == 0)
        return ERROR_OK;

    return ERROR_OK;
}