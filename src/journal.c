#include "journal.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "const.h"
#include "file.h"
#include "logging.h"
#include "table_head.h"
#include "error.h"

#define JOURNAL_STAGED      0b11111111
#define JOURNAL_UNSTAGED    0b00000000
static uint8_t Journal_staged = JOURNAL_STAGED;
static uint8_t Journal_unstaged = JOURNAL_UNSTAGED;

ErrorCode Journal_open(TableHead *th) {
    char journalName[TABLE_NAME_LENGTH_MAX + 1];
    journalName[0] = '$';
    memcpy(journalName + 1, th->name, TABLE_NAME_LENGTH_MAX);

    int fd;
    TRY(FILE_open(&fd, journalName), "Error whilst opening journal file for table \"%s\"", th->name);
    th->journalFilePtr = fd;
    TRY(FILE_truncate(th->journalFilePtr, sizeof(uint8_t)), "Error whilst truncating journal file for table \"%s\"", th->name);
    TRY(FILE_writeAt(th->journalFilePtr, 0, &Journal_unstaged, sizeof(uint8_t)), 
        "Error whilst writing staging information to journal for table \"%s\"", th->name);
    TRY(FILE_seek(th->journalFilePtr, 0, SEEK_END), "Error whilst seeking end of journal for table \"%s\"", th->name);

    LOGGING_debug("Sucessfully opened journal file for table \"%s\"", th->name);
    return ERROR_OK;
}
ErrorCode Journal_write(TableHead *th, page64_t pagePos, uint64_t dataPos, const void *buf, uint64_t n) {
    if (th->journalFilePtr < 0)
        TRY(Journal_open(th), "Error whilst opening journal for table \"%s\"", th->name);
    
    TRY(FILE_write(th->journalFilePtr, &pagePos, sizeof(page64_t)), "Error whilst writing journal page pos for table \"%s\"", th->name);
    TRY(FILE_write(th->journalFilePtr, &dataPos, sizeof(uint64_t)), "Error whilst writing journal data pos for table \"%s\"", th->name);
    TRY(FILE_write(th->journalFilePtr, &n, sizeof(uint64_t)), "Error whilst writing journal data size for table \"%s\"", th->name);
    TRY(FILE_write(th->journalFilePtr, buf, n), "Error whilst writing journal data for table \"%s\"", th->name);

    return ERROR_OK;
}
ErrorCode Journal_stage(TableHead *th) {
    assert(th->journalFilePtr >= 0);

    TRY(FILE_writeAt(th->journalFilePtr, 0, &Journal_staged, sizeof(uint8_t)), 
        "Error whilst writing staging information to journal for table \"%s\"", th->name);
    TRY(FILE_sync(th->journalFilePtr), "Error whilst syncronizing journal for table \"%s\"", th->name);
        
    return ERROR_OK;
}
ErrorCode Journal_commit(TableHead *th) {
    assert(th->journalFilePtr >= 0);

    page64_t pagePos;
    uint64_t dataPos, n;
    uint8_t buf[PAGE_SIZE];
    
    int warnings; ErrorCode e;
    
    TRY(FILE_seek(th->journalFilePtr, 0, SEEK_SET), "Error whilst seeking start of journal file for table \"%s\"", th->name);
    
    uint8_t isStaged;
    TRY(FILE_read(th->journalFilePtr, &isStaged, sizeof(uint8_t), &warnings), 
        "Error whilst reading staging information in journal for table \"%s\"", th->name)
    if (isStaged != JOURNAL_STAGED) {
        LOGGING_error("Journal is not staged for table \"%s\"", th->name);
        return ERROR_JOURNAL_UNSTAGED;
    }
    
    while ((e = FILE_read(th->journalFilePtr, &pagePos, sizeof(page64_t), &warnings)) == ERROR_OK) {
        TRY(FILE_read(th->journalFilePtr, &dataPos, sizeof(uint64_t), &warnings), "Error whilst reading journal data pos for table \"%s\"", th->name)
        TRY(FILE_read(th->journalFilePtr, &n, sizeof(uint64_t), &warnings), "Error whilst reading journal data size for table \"%s\"", th->name)
        TRY(FILE_read(th->journalFilePtr, buf, n, &warnings), "Error whilst reading journal data for table \"%s\"", th->name)
    
        TRY(FILE_writeAt(th->filePtr, (pagePos << PAGE_POW) + dataPos, buf, n), "Error whilst writing datablock to file for table \"%s\"", th->name);
    }
    if (! (warnings & WARNING_EOF)) {
        LOGGING_error("Error whilst reading journal page pos for table \"%s\"", th->name);    
        return e;
    }

    TRY(FILE_sync(th->filePtr), "Error whilst syncronizing file for table \"%s\"", th->name);
    
    TRY(FILE_writeAt(th->journalFilePtr, 0, &Journal_unstaged, sizeof(uint8_t)), 
        "Error whilst writing staging information to journal for table \"%s\"", th->name);
    TRY(FILE_truncate(th->journalFilePtr, sizeof(uint8_t)), "Error whilst truncating journal file for table \"%s\"", th->name);
    TRY(FILE_sync(th->journalFilePtr), "Error whilst syncronizing journal for table \"%s\"", th->name);
    TRY(FILE_seek(th->journalFilePtr, 0, SEEK_END), "Error whilst seeking end of journal for table \"%s\"", th->name);

    return ERROR_OK;
}