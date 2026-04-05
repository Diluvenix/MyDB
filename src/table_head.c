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
    LOGGING_EXECUTE_ON_INFO(uint64_t origKey = key);
    LOGGING_EXECUTE_ON_INFO(uint64_t origValue = value);

    if (th->rootId == 0) {
        // Create new root node
        TableNode newNode = {
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
        uint64_t lowerBound, upperBound, innerBound;
        while (node->flags & TABLE_NODE_IS_INNER_FLAG) {
            lowerBound = 0; upperBound = node->elementCount - 1;

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

                TRY(CACHE_read(th->id, node->values[lowerBound], (void **)&node), "Error whilst retrieving node [%" PRIp64 "] from cache for table \"%s\"", node->values[lowerBound], th->name)
            }
        }

        // Search for key in leaf node
        lowerBound = 0; upperBound = node->elementCount - 1;
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

        TableNode newNode = {
            .flags = node->flags,
            .elementCount = TABLE_NODE_CHILD_SPLIT_MIN,
            .prev = node->id,
            .next = node->next
        };
        // Split node when full
        while (node->elementCount >= TABLE_NODE_CHILD_COUNT) {
            TRY(TableHead_getFree(th, &newNode.id), "Error whilst getting next freeId for table \"%s\"", th->name);

            node->elementCount = TABLE_NODE_CHILD_SPLIT_MAX;
            node->next = newNode.id;

            lowerBound = 0;
            if (node->keys[TABLE_NODE_CHILD_SPLIT_MAX] > key) {
                for (upperBound = TABLE_NODE_CHILD_SPLIT_MAX; upperBound < TABLE_NODE_CHILD_COUNT; upperBound++) {
                    newNode.keys[lowerBound] = node->keys[upperBound];
                    newNode.values[lowerBound++] = node->values[upperBound];
                }
                for (upperBound = TABLE_NODE_CHILD_SPLIT_MAX; node->keys[upperBound - 1] > key && upperBound > 0; upperBound--) {
                    node->keys[upperBound] = node->keys[upperBound - 1];
                    node->values[upperBound] = node->values[upperBound - 1];
                }
                node->keys[upperBound] = key;
                node->values[upperBound] = value;
            }
            else {
                for (upperBound = TABLE_NODE_CHILD_SPLIT_MAX; upperBound < TABLE_NODE_CHILD_COUNT && node->keys[upperBound] < key; upperBound++) {
                    newNode.keys[lowerBound] = node->keys[upperBound];
                    newNode.values[lowerBound++] = node->values[upperBound];
                }
                newNode.keys[lowerBound] = key;
                newNode.values[lowerBound++] = value;
                for (; upperBound < TABLE_NODE_CHILD_COUNT; upperBound++) {
                    newNode.keys[lowerBound] = node->keys[upperBound];
                    newNode.values[lowerBound++] = node->values[upperBound];
                }
            }

            if (node->parent == 0) {
                TRY(TableHead_getFree(th, &newNode.parent), "Error whilst getting next freeId for table \"%s\"", th->name);
                node->parent = newNode.parent;

                TRY(Journal_write(th, node->id, 0, node, PAGE_SIZE), "Error whilst writing node[%" PRIp64 "] to journal for table \"%s\"", node->id, th->name);
                TRY(Journal_write(th, newNode.id, 0, &newNode, PAGE_SIZE), "Error whilst writing node[%" PRIp64 "] to journal for table \"%s\"", newNode.id, th->name);

                if (newNode.flags & TABLE_NODE_IS_INNER_FLAG) {
                    LOGGING_error("New Node Parent update and cache invalidation not yet implemented!");
                    return ERROR_NOT_IMPLEMENTED;
                }

                key = newNode.keys[0];
                value = newNode.id;

                newNode.id = node->parent;
                newNode.flags = TABLE_NODE_IS_INNER_FLAG;
                newNode.elementCount = 1;
                newNode.parent = newNode.prev = newNode.next = 0;
                newNode.keys[0] = node->keys[0];
                newNode.values[0] = node->id;
                node = &newNode;

                th->rootId = node->id;
                TRY(Journal_write(th, 0, offsetof(TableHead, rootId), &th->rootId, sizeof(page64_t)), "Error whilst updating rootId to journal for table \"%s\"", th->name);
                break;
            } else {
                LOGGING_error("Parent Node insertion not yet implemented!");
                return ERROR_NOT_IMPLEMENTED;

                if (newNode.flags & TABLE_NODE_IS_INNER_FLAG) {
                    LOGGING_error("New Node Parent update and cache invalidation not yet implemented!");
                    return ERROR_NOT_IMPLEMENTED;
                }
                
                key = newNode.keys[0];
                value = newNode.id;
            }
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
    LOGGING_info("Sucessfully inserted [%" PRIu64 "]:[%" PRIu64 "] into table \"%s\"", origKey, origValue, th->name);

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
