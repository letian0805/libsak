#ifndef __PROTO_BUFFER_H
#define __PROTO_BUFFER_H
#include <stdint.h>

typedef struct ProtoBuffer ProtoBuffer;

typedef struct{
    int (*header_size)(void);
    int (*read_header)(uint8_t *buf, int fd);
    int (*proto_size)(uint8_t *buf);
}PBFuncs;

struct ProtoBuffer{
    int h_size;
    int h_offset;
    uint8_t *header;

    int d_size;
    int d_offset;
    uint8_t *data;

    PBFuncs funcs;
};

ProtoBuffer *pbuffer_new(int size, PBFuncs funcs);

int pbuffer_fill(ProtoBuffer *pb, int fd);

int pbuffer_get_header(ProtoBuffer *pb, uint8_t **pheader);

int pbuffer_get_data(ProtoBuffer *pb, uint8_t **pdata);

int pbuffer_reset(ProtoBuffer *pb);

void pbuffer_free(ProtoBuffer *pb);

#endif //!__PROTO_BUFFER_H
