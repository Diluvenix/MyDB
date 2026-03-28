#ifndef _JOURNAL_H_
#define _JOURNAL_H_

#include "const.h"
#include "error.h"
#include "table_head.h"

ErrorCode Journal_write(TableHead *th, page64_t pagePos, uint64_t dataPos, const void *buf, uint64_t n);
ErrorCode Journal_stage(TableHead *th);
ErrorCode Journal_commit(TableHead *th);

#endif