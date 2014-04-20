#ifndef __EBUFFER_H
#define __EBUFFER_H

typedef struct EBuffer EBuffer;

struct EBuffer{
    uint32_t size;
    volatile uint32_t offset;
    volatile uint32_t len;
    uint8_t *buf;
};

EBuffer *ebuffer_new(int size);

int ebuffer_set_buffer(EBuffer *ebuf, uint8_t *buf, int size);

uint8_t *ebuffer_get_buffer(EBuffer *ebuf);

int ebuffer_get_data(EBuffer *ebuf, uint8_t *buf, int len);

int ebuffer_put_data(EBuffer *ebuf, uint8_t *data, int len);

int ebuffer_reset(EBuffer *ebuf);

void ebuffer_free(EBuffer *ebuf);

#endif //!__EBUFFER_H
