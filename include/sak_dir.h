#ifndef __SAK_DIR_H
#define __SAK_DIR_H
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef _MAX_FNAME
#define _MAX_FNAME 1024
#endif

typedef enum{
    SAK_FILE_UNKNOWN = 0,
    SAK_FILE_NORMAL,
    SAK_FILE_DIR,
}SakFileType;

typedef struct{
    SakFileType type;
    time_t tm_create;
    time_t tm_write;
    char name[_MAX_FNAME];
}SakDirItem;

typedef struct SakDir SakDir;

SakDir *dir_open(const char *path);

int dir_read(SakDir *dir, SakDirItem *item);

int dir_search(SakDir *dir, const char *name, SakDirItem *item);

int dir_rewind(SakDir *dir);

int dir_close(SakDir *dir);

#endif //!__SAK_DIR_H
