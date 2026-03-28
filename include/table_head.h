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

    int64_t filePtr;
    int64_t journalFilePtr;
} TableHead;

ErrorCode TableHead_open(TableHead *th, const uint8_t *tableName);
ErrorCode TableHead_reopen(TableHead *th);
ErrorCode TableHead_close(TableHead *th);

#endif