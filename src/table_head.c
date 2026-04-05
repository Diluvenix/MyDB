#include "table_head.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "cache.h"
#include "const.h"
#include "error.h"
#include "file.h"
#include "journal.h"
#include "logging.h"
#include "table_node.h"

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
        // Create new root node
        TableNode newNode = {
            .id=0,
            .flags=TABLE_NODE_IS_LEAF_FLAG,
            .elementCount=1,
            .prev=0,
            .next=0,
            .parent=0,
            .keys={key,0},
            .values={value,0}
        };
        TRY(TableHead_getFree(th, &newNode.id), "Error whilst getting next freeId for table \"%s\"", th->name);

        th->rootId = newNode.id;
        TRY(Journal_write(th, newNode.id, 0, &newNode, PAGE_SIZE), "Error whilst writing new node to journal for table \"%s\"", th->name);
        TRY(Journal_write(th, 0, offsetof(TableHead, rootId), &th->rootId, sizeof(page64_t)), "Error whilst updating rootId to journal for table \"%s\"", th->name);
    }
    else {
        TableNode *node;
        TRY(CACHE_read(th->id, th->rootId, (void **)&node), "Error whilst retrieving root node from cache for table \"%s\"", th->name);
        
        // Decend into correct leaf node
        while (!(node->flags & TABLE_NODE_IS_LEAF_FLAG)) {
            LOGGING_error("Node decend not implemented!");
            return ERROR_NOT_IMPLEMENTED;
        }

        // Search for key in leaf node
        uint64_t lowerBound = 0, upperBound = node->elementCount - 1, innerBound;
        while (lowerBound < upperBound) {
            innerBound = lowerBound + ((upperBound - lowerBound + 1) >> 1);
            if (node->keys[innerBound] < key) {
                lowerBound = innerBound;
            } else if (node->keys[innerBound] > key) {
                upperBound = innerBound - 1;
            } else {
                lowerBound = innerBound;
                break;
            }
        }

        // Check for key already exists
        if (node->keys[lowerBound] == key) {
            LOGGING_warning("Key [%" PRIu64 "] already in table \"%s\"", key, th->name);
            return ERROR_TABLE_INSERTION;
        }

        // Split node when full
        while (node->elementCount >= TABLE_NODE_CHILD_COUNT) {
            LOGGING_error("Node splitting not implemented!");
            return ERROR_NOT_IMPLEMENTED;
        }

        // Final key value insertion
        for (upperBound = node->elementCount; node->keys[upperBound - 1] > key && upperBound > 0; upperBound--) {
            node->keys[upperBound] = node->keys[upperBound - 1];
            node->values[upperBound] = node->values[upperBound - 1];
        }
        node->keys[upperBound] = key;
        node->values[upperBound] = value;
        node->elementCount += 1;
        TRY(Journal_write(th, node->id, 0, node, PAGE_SIZE), "Error whilst writing node[%" PRIp64 "] to journal for table \"%s\"", node->id, th->name);
    }

    TRY(Journal_stage(th), "Error whilst staging changes to table \"%s\"", th->name);
    TRY(Journal_commit(th), "Error whilst committing changes to table \"%s\"", th->name);
    LOGGING_info("Sucessfully inserted [%" PRIu64 "]:[%" PRIu64 "] into table \"%s\"", key, value, th->name);

    return ERROR_OK;
}

ErrorCode TableHead_getFree(TableHead *th, page64_t *id) {
    if (th->freeId == 0) {
        *id = th->nextId++;
        TRY(Journal_write(th, 0, offsetof(TableHead, nextId), &th->nextId, sizeof(page64_t)), "Error whilst updating nextId to journal for table \"%s\"", th->name);
    } else {
        LOGGING_error("Free page usage not yet implemented");
        return ERROR_NOT_IMPLEMENTED;
    }
    return ERROR_OK;
}
