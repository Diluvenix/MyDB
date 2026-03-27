#include "journal.h"
#include "error.h"

ErrorCode Journal_open(Journal *jrn, const char *destFileName) {
    return ERROR_OK;
}
ErrorCode Journal_write(Journal *jrn, uint64_t pagePos, uint64_t dataPos, const void *buf, page64_t n) {
    return ERROR_OK;
}
ErrorCode Journal_commit(Journal *jrn) {
    return ERROR_OK;
}