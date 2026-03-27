#ifndef _CONST_H_
#define _CONST_H_

#include <stdint.h>

#define PAGE_SIZE   (128)
typedef uint64_t page64_t;

#define TABLE_NAME_LENGTH_MIN   (3)
#define TABLE_NAME_LENGTH_MAX   ((PAGE_SIZE) - (5 * 8))

#endif