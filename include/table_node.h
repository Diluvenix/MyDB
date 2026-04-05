#ifndef _TABLE_NODE_H_
#define _TABLE_NODE_H_

#include "const.h"
#include <stdint.h>

typedef struct s_TableNode {
    page64_t id;
    uint64_t flags;
    uint64_t elementCount;

    page64_t prev;
    page64_t next;
    page64_t parent;

    uint64_t keys[TABLE_NODE_CHILD_COUNT];
    uint64_t values[TABLE_NODE_CHILD_COUNT];
} TableNode;

#endif