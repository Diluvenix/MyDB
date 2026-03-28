#ifndef _JOURNAL_H_
#define _JOURNAL_H_

#include <stddef.h>
#include "const.h"
#include "error.h"
#include "table_head.h"

ErrorCode Journal_write(TableHead *th, page64_t pagePos, uint64_t dataPos, const void *buf, size_t n);
ErrorCode Journal_commit(TableHead *th);

#endif