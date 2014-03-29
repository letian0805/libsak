#ifndef __MEM_POOL_H
#define __MEM_POOL_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MemPool MemPool;

MemPool *mempool_new(int block_size);

uint64_t mempool_memsize(MemPool *mp);

int mempool_blksize(MemPool *mp);

void *mempool_get(MemPool *mp);

void mempool_put(MemPool *mp, void *mem);

void mempool_free(MemPool *mp);

#ifdef __cplusplus
}
#endif

#endif //!__MEM_POOL_H
