#ifndef __MEM_POOL_H
#define __MEM_POOL_H
#include <stdint.h>

typedef struct MemPool MemPool;

MemPool *mempool_new(int block_size, int blocks_per_chunk);

uint64_t mempool_size(MemPool *mp);

void *mempool_get(MemPool *mp);

void mempool_put(MemPool *mp, void *mem);

void mempool_free(MemPool *mp);


#endif //!__MEM_POOL_H
