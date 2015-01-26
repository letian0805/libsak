#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sak.h"

#ifdef __windows__
#include <windows.h>
#define LIB_SUFFIX ".dll"
typedef HINSTANCE SakDlHandle;
#define DL_OPEN(_name) LoadLibrary(_name)
#define DL_GET(_handler, _symbol) GetProcAddress(_handler, _symbol)
#define DL_CLOSE(_handler) FreeLibrary(_handler)
#else
#include <unistd.h>
#include <dlfcn.h>
#define LIB_SUFFIX ".so"
typedef void * SakDlHandle;
#define DL_OPEN(_name)  dlopen(_name, RTLD_LAZY)
#define DL_GET(_handler, _symbol) dlsym(_handler, _symbol)
#define DL_CLOSE(_handler) dlclose(_handler)
#endif

struct SakDl{
    char *path;
    SakDlHandle handle;
};

SakDl *sak_dl_open(const char *dlfile)
{
    assert(dlfile);
    SakDlHandle handle = DL_OPEN(dlfile);
    if (handle == (SakDlHandle)NULL){
        return NULL;
    }
    SakDl *dl = (SakDl *)malloc(sizeof(SakDl));
    dl->handle = handle;
    dl->path = strdup(dlfile);

    return dl;
}

void *sak_dl_getsymbol(SakDl *dl, const char *symbol_name)
{
    assert(dl);
    assert(symbol_name);

    return DL_GET(dl->handle, symbol_name);
}

void sak_dl_close(SakDl *dl)
{
    if (dl){
        free(dl->path);
        DL_CLOSE(dl->handle);
    }
}

