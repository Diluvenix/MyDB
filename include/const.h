#ifndef _CONST_H_
#define _CONST_H_

#include <stdint.h>

#define PAGE_POW    (7)
#define PAGE_SIZE   (1 << (PAGE_POW))
typedef uint64_t page64_t;

#define TABLE_NAME_LENGTH_MIN   (3)
#define TABLE_NAME_LENGTH_MAX   ((PAGE_SIZE) - (5 * 8))

#endif