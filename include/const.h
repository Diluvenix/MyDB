#ifndef _CONST_H_
#define _CONST_H_

#include <stdint.h>
#include <inttypes.h>

#define PAGE_POW    (7)
#define PAGE_SIZE   (1 << (PAGE_POW))
typedef uint64_t page64_t;
#define PRIp64 PRIu64

#define TABLE_NAME_LENGTH_MIN   (3)
#define TABLE_NAME_LENGTH_MAX   ((PAGE_SIZE) - (6 * 8))

#define TABLE_NODE_CHILD_COUNT  (((PAGE_SIZE) - (6 * 8)) / 16)
#define TABLE_NODE_IS_LEAF_FLAG (1 << 0)

#endif