#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>

#include "mempool.h"
#include "stack.h"
#include "meminfo.h"
#include "log.h"

#define MINSLOTSIZE 32
#define MAXCHUNKSIZE (2*1024*1024)
#define MINBLOCKSIZE 32
#define BLKSIZEALIGN 8

struct MemPool{
    uint64_t mem_size;
    int blocks_per_chunk;
    int block_size;
    int chunk_size;
    int nchunk;
    int slot_size;
    uint8_t **slot;
    int free_chunk_index;
    int free_block_index;
    Stack *stack;
    pthread_spinlock_t lock;
};

static inline void mempool_enlarge_slot(MemPool *mp)
{
    int new_slot_size = mp->slot_size * 2;
    uint8_t **new_slot = (uint8_t **)realloc(mp->slot, new_slot_size);
    if (new_slot){
        mp->mem_size += new_slot_size - mp->slot_size;
        mp->slot = new_slot;
        mp->slot_size = new_slot_size;
    }
}

static inline void mempool_add_chunk(MemPool *mp)
{
    uint8_t *chunk = (uint8_t *)malloc(mp->chunk_size);
    if (mp->nchunk == mp->slot_size){
        mempool_enlarge_slot(mp);
    }
    mp->mem_size += mp->chunk_size;
    mp->free_chunk_index = mp->nchunk;
    mp->free_block_index = 0;
    mp->slot[mp->nchunk++] = chunk;
}

int mempool_make_blksize(int size)
{
    int align_mask = BLKSIZEALIGN - 1;
    
    size += sizeof(MemBlkInfo);
    if (size < MINBLOCKSIZE){
        size = MINBLOCKSIZE;
    }
    if ( (size & align_mask) ){
        size = ((size + align_mask) & ~align_mask);
    }

    return size;
}

MemPool *mempool_new(int block_size)
{
    int blocks_per_chunk = 0;
    MemPool *mp = (MemPool *)malloc(sizeof(MemPool));
    mp->mem_size = sizeof(MemPool);

    mp->block_size = mempool_make_blksize(block_size);
    mp->chunk_size = MAXCHUNKSIZE;
    mp->blocks_per_chunk = mp->chunk_size/mp->block_size;

    mp->nchunk = 0;
    mp->slot_size = MINSLOTSIZE;
    mp->slot = (uint8_t **)malloc(sizeof(uint8_t *) * mp->slot_size);
    mp->mem_size += sizeof(uint8_t *) * mp->slot_size;
    mempool_add_chunk(mp);
    mp->stack = stack_new(1024 * sizeof(void *));
    mp->mem_size += stack_size(mp->stack);

    pthread_spin_init(&mp->lock, 0);
    return mp;  
}

uint64_t mempool_memsize(MemPool *mp)
{
    return mp->mem_size;
}

int mempool_blksize(MemPool *mp)
{
    if (!mp){
        return -1;
    }
    return mp->block_size;
}

void *mempool_get(MemPool *mp)
{
    uint8_t *mem = NULL;
    stack_pop(mp->stack, &mem, sizeof(mem));
    if (mem){
        return mem;
    }

    if (mp->free_block_index >= mp->blocks_per_chunk){
        mempool_add_chunk(mp);
    }
    mem = mp->slot[mp->free_chunk_index] + mp->block_size * mp->free_block_index;
    MemBlkInfo *minfo = (MemBlkInfo *)mem;
    meminfo_init(BLK_TYPE_POOL, (MemInfo *)minfo);
    minfo->chunk_index = mp->free_chunk_index;
    minfo->block_index = mp->free_block_index;
    mp->free_block_index++;

    mem += sizeof(MemBlkInfo);
    return (void *)mem;
}

void mempool_put(MemPool *mp, void *mem)
{
    if (stack_full(mp->stack)){
        int size = stack_size(mp->stack);
        stack_enlarge(mp->stack, 1.5);
        mp->mem_size += stack_size(mp->stack) - size;
    }
    DEBUG("---------put: %p", mem);
    stack_push(mp->stack, &mem, sizeof(mem));
}

void mempool_free(MemPool *mp)
{
    stack_free(mp->stack);
    int i;
    for (i = 0; i < mp->nchunk; i++){
        free(mp->slot[i]);
    }
    free(mp->slot);
    pthread_spin_destroy(&mp->lock);
    free(mp);
}

