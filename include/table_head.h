#ifndef _TABLE_HEAD_H_
#define _TABLE_HEAD_H_

#include <stdint.h>
#include "const.h"
#include "error.h"

typedef struct s_TableHead {
    uint64_t id;
    uint8_t name[TABLE_NAME_LENGTH_MAX];

    page64_t rootId;
    page64_t nextId;
    page64_t freeId;

    int64_t filePtr;
    int64_t journalFilePtr;
} TableHead;

ErrorCode TableHead_reopen(TableHead *th);
ErrorCode TableHead_close(TableHead *th);

ErrorCode TableHead_insertKeyValue(TableHead *th, uint64_t key, uint64_t value);
ErrorCode TableHead_getFree(TableHead *th, page64_t *id);

#endif