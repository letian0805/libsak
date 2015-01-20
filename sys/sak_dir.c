#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sak.h"

#ifdef __windows__
#include <io.h>
#include <windows.h>
#include <time.h>
typedef HANDLE SakDirHandle;
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
typedef DIR * SakDirHandle;
#endif

struct SakDir{
    char *path;
    bool is_first;
    SakDirItem first_item;
    SakDirHandle handle;
};

SakDir *sak_dir_open(const char *path)
{
    assert(path);
    assert(*path);
    SakDir *dir = (SakDir *)malloc(sizeof(SakDir));
#ifdef __windows__
    WIN32_FIND_DATA fileinfo;
    time_t tm;
    ULARGE_INTEGER ull;
    char to_search[4096];
    sprintf(to_search, "%s\\*", path);
    dir->handle = FindFirstFile(to_search, &fileinfo);
    if (dir->handle < 0){
        free(dir);
	    printf("can't open dir: %s\n", path);
        return NULL;
    }
    while(fileinfo.cFileName[0] == '.'){
        if(FindNextFile(dir->handle, &fileinfo) <= 0){
            return NULL;
        }
    }
    ull.LowPart = fileinfo.ftLastWriteTime.dwLowDateTime;
    ull.HighPart = fileinfo.ftLastWriteTime.dwHighDateTime;
    tm = ull.QuadPart / 10000000ULL - 11644473600ULL;
    dir->first_item.tm_write = tm;
    if ( (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
        dir->first_item.type = SAK_FILE_DIR;
    } else{
        dir->first_item.type = SAK_FILE_NORMAL;
    } 
    strcpy(dir->first_item.name, fileinfo.cFileName);
#else
    dir->handle = opendir(path);
    if (dir->handle == NULL){
        free(dir);
        return NULL;
    }
    struct dirent entry;
    struct dirent *result = NULL;
    char file_path[4096];
    struct stat st;
RETRY:
    if (readdir_r(dir->handle, &entry, &result) != 0 || result == NULL){
        closedir(dir->handle);
        free(dir);
        return NULL;
    }

    sprintf(file_path, "%s/%s", path, entry.d_name);
    if (stat(file_path, &st)!=0){
        goto RETRY;
    }

    if (entry.d_type == DT_DIR){
        dir->first_item.type = SAK_FILE_DIR;
    }else if(entry.d_type == DT_REG){
        dir->first_item.type = SAK_FILE_NORMAL;
    }else{
        dir->first_item.type = SAK_FILE_UNKNOWN;
    }
    dir->first_item.tm_write = st.st_mtime;
    strcpy(dir->first_item.name, entry.d_name);
#endif
    dir->is_first = true;
    dir->path = strdup(path);
    return dir;
}

int sak_dir_read(SakDir *dir, SakDirItem *item)
{
    assert(dir);
    assert(item);
    if (dir->is_first == true){
        *item = dir->first_item;
        dir->is_first = false;
    }else{
#ifdef __windows__
        WIN32_FIND_DATA fileinfo;
        time_t tm;
        ULARGE_INTEGER ull;
RETRY:
        if (FindNextFile(dir->handle, &fileinfo) <= 0){
            return -1;
        }
        if ( (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
            dir->first_item.type = SAK_FILE_DIR;
        } else {
            dir->first_item.type = SAK_FILE_NORMAL;
        }
        if (fileinfo.cFileName[0] == '.'){
            goto RETRY;
        }
        strcpy(item->name, fileinfo.cFileName);
        ull.LowPart = fileinfo.ftLastWriteTime.dwLowDateTime;
        ull.HighPart = fileinfo.ftLastWriteTime.dwHighDateTime;
        tm = ull.QuadPart / 10000000ULL - 11644473600ULL;
        item->tm_write = tm;
#else
        struct dirent entry;
        struct dirent *result = NULL;
        char file_path[4096];
        struct stat st;
RETRY:
        if (readdir_r(dir->handle, &entry, &result) != 0 || result == NULL){
            return -1;
        }

        sprintf(file_path, "%s/%s", dir->path, entry.d_name);
        if (stat(file_path, &st)!=0){
            goto RETRY;
        }

        if (entry.d_type == DT_DIR){
            item->type = SAK_FILE_DIR;
        }else if(entry.d_type == DT_REG){
            item->type = SAK_FILE_NORMAL;
        }else{
            item->type = SAK_FILE_UNKNOWN;
        }
        item->tm_write = st.st_mtime;
        strcpy(item->name, entry.d_name);
#endif
    }
    return 0;
}

int sak_dir_search(SakDir *dir, const char *name, SakDirItem *item)
{
    SakDirItem tmp;
    int ret = -1;

    assert(dir);
    assert(name);
    assert(item);

    while(sak_dir_read(dir, &tmp) == 0){
        if (strcmp(name, tmp.name) == 0){
            *item = tmp;
            ret = 0;
            break;
        }
    }
    return ret;
}

int sak_dir_rewind(SakDir *dir)
{
    assert(dir);

#ifdef __windows__

#else
    rewinddir(dir->handle);
#endif
    return 0;
}

int sak_dir_close(SakDir *dir)
{
    assert(dir);
#ifdef __windows__

#else
    closedir(dir->handle);
#endif
    free(dir->path);
    free(dir);
    return 0;
}

