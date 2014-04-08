#ifndef __EFILE_H
#define __EFILE_H
#include "epool.h"

typedef struct EFile EFile;
struct EFile{
    int proto;
    int state;
    int need_data_len;
    int get_data_len;
    EFileFuncs funcs;
    void *owner;
};

typedef struct{
    int (*evevt_fd)(void *owner);
    int (*event_type)(void *owner);
    int (*read)(void *owner, uint8_t *buf, int len);
    int (*write)(void *owner, uint8_t *buf, int len);
    int (*handler)(EFile *ef, int events, void *owner);
}EFileFuncs;

EFile *efile_open(void *owner, EFileFuncs *funcs, int enable_proto);

int efile_read(EFile *ef, uint8_t *buf, int len);

int efile_write(EFile *ef, uint8_t *buf, int len);

void efile_close(EFile *ef);

#endif //!__EFILE_H
