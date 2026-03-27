#ifndef _JOURNAL_H_
#define _JOURNAL_H_

#include <stddef.h>
#include "const.h"
#include "error.h"
#include "table_head.h"

ErrorCode Journal_open(TableHead *th, const char *destFileName);
ErrorCode Journal_write(const TableHead *th, page64_t pagePos, uint64_t dataPos, const void *buf, page64_t n);
ErrorCode Journal_commit(TableHead *th);

#endif