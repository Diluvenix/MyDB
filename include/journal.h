#ifndef _JOURNAL_H_
#define _JOURNAL_H_

#include <stddef.h>
#include <stdint.h>
#include "error.h"

typedef struct s_Journal {
    int destinationFilePtr;
    int journalFilePtr;
} Journal;

ErrorCode Journal_open(Journal *jrn, const char *destFileName);
ErrorCode Journal_write(Journal *jrn, uint64_t blockPos, uint64_t dataPos, const void *buf, size_t n);
ErrorCode Journal_commit(Journal *jrn);

#endif