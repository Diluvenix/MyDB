#include "journal.h"
#include "const.h"
#include "file.h"
#include "logging.h"
#include "table_head.h"
#include "error.h"

ErrorCode Journal_open(TableHead *th) {
    char journalName[TABLE_NAME_LENGTH_MAX + 1];
    journalName[0] = '$';
    memcpy(journalName + 1, th->name, TABLE_NAME_LENGTH_MAX);

    int fd;
    TRY(FILE_open(&fd, journalName), "Error whilst opening journal file for table \"%s\"", th->name);
    th->journalFilePtr = fd;
    TRY(FILE_truncate(th->journalFilePtr, 0), "Error whilst truncating journal file for table \"%s\"", th->name);

    LOGGING_debug("Sucessfully opened journal file for table \"%s\"", th->name);
    return ERROR_OK;
}
ErrorCode Journal_write(TableHead *th, page64_t pagePos, uint64_t dataPos, const void *buf, size_t n) {
    if (th->journalFilePtr < 0)
        TRY(Journal_open(th), "Error whilst opening journal for table \"%s\"", th->name);

    return ERROR_OK;
}
ErrorCode Journal_commit(TableHead *th) {
    return ERROR_OK;
}