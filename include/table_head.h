#ifndef _TABLE_HEAD_H_
#define _TABLE_HEAD_H_

#include <stdint.h>
#include "const.h"
#include "error.h"
#include "journal.h"

typedef struct s_TableHead {
    uint64_t id;
    uint8_t name[TABLE_NAME_LENGTH_MAX];

    uint64_t rootId;
    uint64_t nextId;

    Journal journal;
} TableHead;

ErrorCode TableHead_open(TableHead *th, const uint8_t *tableName);

#endif