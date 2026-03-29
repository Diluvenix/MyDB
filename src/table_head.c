#include "table_head.h"

#include <assert.h>
#include <stddef.h>
#include "const.h"
#include "error.h"
#include "file.h"
#include "journal.h"
#include "logging.h"
#include "table_node.h"

ErrorCode TableHead_open(TableHead *th, const uint8_t *tableName) {
    return ERROR_OK;
}

ErrorCode TableHead_reopen(TableHead *th) {
    assert(th->name[0] != '\0');
    
    int fd, warnings = 0; ErrorCode e;
    TRY(FILE_open(&fd, (char *)th->name), "Error whilst opening file for table \"%s\"", th->name);
    if ((e = FILE_read(fd, th, PAGE_SIZE, &warnings)) != ERROR_OK && warnings == WARNING_NONE) {
        LOGGING_error("Error whilst reading table head from file for table \"%s\"", th->name);
        return e;
    } else if (warnings & WARNING_EOF) {
        th->filePtr = fd;
        TRY(Journal_write(th, 0, 0, th, PAGE_SIZE), "Error whilst writing table head to journal for table \"%s\"", th->name);
        TRY(Journal_stage(th), "Error whilst staging table head to journal for table \"%s\"", th->name);
        TRY(Journal_commit(th), "Error whilst committing table head to journal for table \"%s\"", th->name);
    }
    else {
        th->journalFilePtr = -1;
        th->filePtr = fd;
    }

    LOGGING_info("Sucessfully reopened table head of table \"%s\"", th->name);
    return ERROR_OK;
}

ErrorCode TableHead_close(TableHead *th) {
    if (th->journalFilePtr >= 0) {
        TRY(FILE_close(th->journalFilePtr), "Error whilst closing journal file for table \"%s\"", th->name);
        LOGGING_debug("Sucessfully closed journal file for table \"%s\"", th->name);
    }
    if (th->filePtr >= 0){
        TRY(FILE_close(th->filePtr), "Error whilst closing file for table \"%s\"", th->name);
        LOGGING_debug("Sucessfully closed file for table \"%s\"", th->name);
    }

    return ERROR_OK;
}

ErrorCode TableHead_insertKeyValue(TableHead *th, uint64_t key, uint64_t value) {
    if (th->rootId == 0) {
        TableNode newNode = {
            .id=th->nextId++,
            .flags=TABLE_NODE_IS_LEAF_FLAG,
            .elementCount=1,
            .prev=0,
            .next=0,
            .parent=0,
            .keys={key,0},
            .values={value,0}
        };
        TRY(Journal_write(th, newNode.id, 0, &newNode, PAGE_SIZE), "Error whilst writing new node to journal for table \"%s\"", th->name);
        TRY(Journal_write(th, 0, offsetof(TableHead, rootId), &newNode.id, sizeof(page64_t)), "Error whilst updating rootId to journal for table \"%s\"", th->name);
        TRY(Journal_write(th, 0, offsetof(TableHead, nextId), &th->nextId, sizeof(page64_t)), "Error whilst updating nextId to journal for table \"%s\"", th->name);
    }
    else {
        return ERROR_NOT_IMPLEMENTED;
    }
    
    TRY(Journal_stage(th), "Error whilst staging changes to table \"%s\"", th->name);
    TRY(Journal_commit(th), "Error whilst committing changes to table \"%s\"", th->name);
    LOGGING_info("Sucessfully inserted [%" PRIu64 "]:[%" PRIu64 "] into table \"%s\"", key, value, th->name);

    return ERROR_OK;
}

