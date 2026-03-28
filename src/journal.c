#include "journal.h"
#include "table_head.h"
#include "error.h"

ErrorCode Journal_open(TableHead *th, const char *destFileName) {
    return ERROR_OK;
}
ErrorCode Journal_write(const TableHead *th, page64_t pagePos, uint64_t dataPos, const void *buf, size_t n) {
    return ERROR_OK;
}
ErrorCode Journal_commit(TableHead *th) {
    return ERROR_OK;
}