#ifndef __SAK_DIR_H
#ifndef __SAK_DEV
#error Do not include this file, please include sak.h instead!!!
#endif

#include <sak/sak_sys.h>

#define __SAK_DIR_H

#ifdef __cplusplus
extern "C" {
#endif

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
    SakTime tm_create;
    SakTime tm_write;
    char name[_MAX_FNAME];
}SakDirItem;

typedef struct SakDir SakDir;

SakDir *sak_dir_open(const char *path);

int sak_dir_read(SakDir *dir, SakDirItem *item);

int sak_dir_search(SakDir *dir, const char *name, SakDirItem *item);

int sak_dir_rewind(SakDir *dir);

int sak_dir_close(SakDir *dir);

#ifdef __cplusplus
}
#endif

#endif //!__SAK_DIR_H
