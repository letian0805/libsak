#ifndef __EFILE_H
#define __EFILE_H
#include "epool.h"
#include "ebuffer.h"

typedef struct EFile EFile;
struct EFile{
    int fd;
    uint32_t rd_offset;
    uint32_t wr_offset;
};

EFile *efile_open(int fd);

int efile_fill(EFile *ef);

int efile_read(EFile *ef, uint8_t *buf, int len);

int efile_write(EFile *ef, uint8_t *buf, int len);

int efile_seek(EFile *ef, 

int efile_sync(EFile *ef);

#endif //!__EFILE_H
