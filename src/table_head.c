#include "table_head.h"

#include <assert.h>
#include "error.h"

ErrorCode TableHead_open(TableHead *th, const uint8_t *tableName) {
    return ERROR_OK;
}

ErrorCode TableHead_reopen(TableHead *th) {
    assert(th->name[0] != '\0');
    
    return ERROR_OK;
}
