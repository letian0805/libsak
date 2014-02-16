#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>

#include "mempool.h"
#include "stack.h"
#include "log.h"

#define DEFCHUNKS 32
#define MINCHUNKSIZE (1024*2100)
#define MINBLOCKSIZE 32

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

typedef struct{
    uint32_t block_index:16;
    uint32_t chunk_index:14;
    uint32_t block_state:2;
}MemBlockInfo;

typedef struct{
    MemBlockInfo blk_info;
    MemPool *mp;
}MemInfo;

static inline void mempool_enlarge_slot(MemPool *mp)
{
    int new_slot_size = mp->slot_size * 2;
    uint8_t **new_slot = (uint8_t **)realloc(mp->slot, new_slot_size);
    if (new_slot){
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

MemPool *mempool_new(int block_size, int blocks_per_chunk)
{
    MemPool *mp = (MemPool *)malloc(sizeof(MemPool));
    
    block_size += sizeof(MemInfo);
    if (block_size < MINBLOCKSIZE){
        block_size = MINBLOCKSIZE;
    }
    if ( (block_size & 7) ){
        block_size = ((block_size + 7) & ~7);
    }
    DEBUG("---------sizeof(MemInfo): %d", sizeof(MemInfo));
    mp->block_size = block_size;
    mp->chunk_size = mp->block_size * blocks_per_chunk;
    if (mp->chunk_size < MINCHUNKSIZE){
        mp->blocks_per_chunk = MINCHUNKSIZE/block_size + 1;
        mp->chunk_size = mp->block_size * mp->blocks_per_chunk;
    }
    mp->nchunk = 0;
    mp->slot_size = DEFCHUNKS;
    mp->slot = (uint8_t **)malloc(sizeof(uint8_t *) * mp->slot_size);
    mempool_add_chunk(mp);
    mp->stack = stack_new(1024 * sizeof(void *));

    pthread_spin_init(&mp->lock, 0);
    return mp;  
}

uint64_t mempool_size(MemPool *mp)
{
    return mp->mem_size;
}

void *mempool_get(MemPool *mp)
{
    uint8_t *mem = NULL;
    stack_pop(mp->stack, &mem, sizeof(mem));
    if (mem){
        return mem;
    }

    MemInfo *minfo = (MemInfo *)(mp->slot[mp->free_chunk_index] + mp->block_size * mp->free_block_index);
    minfo->blk_info.block_index = mp->free_block_index;
    minfo->mp = mp;
    mem = (uint8_t *)minfo + sizeof(MemInfo);
    mp->free_block_index++;
    if (mp->free_block_index >= mp->blocks_per_chunk){
        mempool_add_chunk(mp);
    }

    return (void *)mem;
}

void mempool_put(MemPool *mp, void *mem)
{
    if (stack_full(mp->stack)){
        stack_enlarge(mp->stack, 1.5);
    }
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

