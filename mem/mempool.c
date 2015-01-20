#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include "sak.h"

#include "mempool.h"
#include "stack.h"
#include "meminfo.h"

#define MINSLOTSIZE 32
#define MAXCHUNKSIZE (4*1024*1024)
#define MINBLOCKSIZE 32
#define BLKSIZEALIGN 8

typedef struct MemChunk MemChunk;
struct MemChunk{
    Stack *stack;
    int blk_size;
    int nblks;
    int free_blks;
    int free_blk_idx;
    uint8_t *mem;
};

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
    uint8_t **new_slot = (uint8_t **)realloc(mp->slot, new_slot_size * sizeof(uint8_t *));
    if (new_slot){
        mp->mem_size += (new_slot_size - mp->slot_size) * sizeof(uint8_t *) ;
        mp->slot = new_slot;
        mp->slot_size = new_slot_size;
    }
}

static inline int mempool_add_chunk(MemPool *mp)
{
    uint8_t *chunk = (uint8_t *)mmap(NULL, mp->chunk_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
    if (chunk == MAP_FAILED){
        return -1;
    }
    if (mp->nchunk == mp->slot_size){
        mempool_enlarge_slot(mp);
    }
    mp->mem_size += mp->chunk_size;
    mp->free_chunk_index = mp->nchunk;
    mp->free_block_index = 0;
    mp->slot[mp->nchunk++] = chunk;

    return 0;
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
    MemPool *mp = (MemPool *)calloc(1, sizeof(MemPool));
    if (!mp){
        return NULL;
    }
    mp->mem_size = sizeof(MemPool);

    mp->block_size = mempool_make_blksize(block_size);
    mp->chunk_size = MAXCHUNKSIZE;
    mp->blocks_per_chunk = mp->chunk_size/mp->block_size;

    mp->nchunk = 0;
    mp->slot_size = MINSLOTSIZE;
    mp->slot = (uint8_t **)calloc(mp->slot_size, sizeof(uint8_t *));
    if (!mp->slot){
        goto ERROR;
    }
    mp->mem_size += sizeof(uint8_t *) * mp->slot_size;
    if (mempool_add_chunk(mp) != 0){
        goto ERROR;
    }
    mp->stack = stack_new(1024 * sizeof(void *));
    if (!mp->stack){
        goto ERROR;
    }
    mp->mem_size += stack_size(mp->stack);

    pthread_spin_init(&mp->lock, 0);
    return mp;

ERROR:
    mempool_free(mp);
    return NULL;
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
    MemBlkInfo *minfo = NULL;
    stack_pop(mp->stack, &minfo, sizeof(minfo));
    if (minfo){
        minfo->type = MEM_TYPE_POOL;
        return minfo->mem;
    }

    if (mp->free_block_index >= mp->blocks_per_chunk){
        mempool_add_chunk(mp);
    }
    minfo = (MemBlkInfo *)(mp->slot[mp->free_chunk_index] + mp->block_size * mp->free_block_index);
    minfo->type = MEM_TYPE_POOL;
    minfo->chunk_index = mp->free_chunk_index;
    minfo->block_index = mp->free_block_index;
    mp->free_block_index++;

    return (void *)minfo->mem;
}

void mempool_put(MemPool *mp, void *mem)
{
    MemBlkInfo *minfo = (MemBlkInfo *)(((uint8_t *)mem) - sizeof(MemBlkInfo));
    if (stack_full(mp->stack)){
        int size = stack_size(mp->stack);
        stack_enlarge(mp->stack, 1.5);
        mp->mem_size += stack_size(mp->stack) - size;
    }
    if (minfo->chunk_index < mp->nchunk){
        stack_push(mp->stack, &minfo, sizeof(minfo));
    }
}

void mempool_free(MemPool *mp)
{
    if (!mp){
        return;
    }
    if (mp->stack){
        stack_free(mp->stack);
    }
    if (mp->slot){
        int i;
        for (i = 0; i < mp->nchunk; i++){
            munmap(mp->slot[i], mp->chunk_size);
        }
        free(mp->slot);
    }
    pthread_spin_destroy(&mp->lock);
    free(mp);
}

