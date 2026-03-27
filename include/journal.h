#ifndef _JOURNAL_H_
#define _JOURNAL_H_

#include <stddef.h>
#include "const.h"
#include "error.h"

typedef struct s_Journal {
    int destinationFilePtr;
    int journalFilePtr;
} Journal;

ErrorCode Journal_open(Journal *jrn, const char *destFileName);
ErrorCode Journal_write(Journal *jrn, uint64_t pagePos, uint64_t dataPos, const void *buf, page64_t n);
ErrorCode Journal_commit(Journal *jrn);

#endif