#include <stdlib.h>

#include "mem.h"
#include "meminfo.h"
#include "mempool.h"
#include "log.h"

static MemPool *mpool_slots[MAXMEMPOOL] = {NULL};

int mem_add_pool(int blk_size)
{
    int id;
    for (id = 0; id < MAXMEMPOOL; id++){
        if (mpool_slots[id] == NULL){
            mpool_slots[id] = mempool_new(blk_size);
            return id;
        }
    }

    return -1;
}

void mem_del_pool(int blksize)
{
    blksize = mempool_make_blksize(blksize);
    int id;
    for (id = 0; id < MAXMEMPOOL; id++){
        MemPool *mp = mpool_slots[id];
        if (mp && blksize == mempool_blksize(mp)){
            mempool_free(mp);
            mpool_slots[id] = NULL;
        }
    }
}

void mem_del_pool_by_id(int mpid)
{
    if (mpid < 0 || mpid >= MAXMEMPOOL){
        return;
    }
    if (mpool_slots[mpid]){
        mempool_free(mpool_slots[mpid]);
        mpool_slots[mpid] = NULL;
    }
}

void *mem_malloc(int size)
{
    uint8_t *mem = NULL;
    size += sizeof(MemBlkInfo);
    MemInfo *minfo = (MemInfo *)malloc(size);
    minfo->type = MEM_TYPE_ALC;
    mem = (uint8_t *)minfo;
    mem += sizeof(MemInfo);
    return (void *)mem;
}

void *mem_map(int size)
{
    return NULL;
}

void *mem_shmget(int size)
{
    return NULL;
}

void *mem_poolget(int size)
{
    uint8_t *mem = NULL;
    MemPool *mp = NULL;
    int blk_size = mempool_make_blksize(size);
    int id;
    for(id = 0; id < MAXMEMPOOL; id++){
        mp = mpool_slots[id];
        if (mp == NULL){
            continue;
        }
        DEBUG("-------mp: %p, blk_size: %d\n", mp, mempool_blksize(mp));
        if (blk_size == mempool_blksize(mp)){
            break;
        }
        mp = NULL;
    }
    id = (id < MAXMEMPOOL)?id:mem_add_pool(size);
    mp = mpool_slots[id];
    mem = mempool_get(mp);
    DEBUG("-------mp: %p. blk size: %d, mem: %p", mp, size, mem);
    if (mem){
        MemBlkInfo *minfo = (MemBlkInfo *)(mem - sizeof(MemBlkInfo));
        minfo->type = MEM_TYPE_POOL;
        minfo->pool_index = id;
    }

    return mem;
}

int mem_size(void *addr)
{
    return 0;
}

void mem_free(void *addr)
{
    MemInfo *minfo = (MemInfo *)((uint8_t *)addr - sizeof(MemBlkInfo));
    switch(minfo->type){
        case MEM_TYPE_ALC:
            break;
        case MEM_TYPE_MAP:
            break;
        case MEM_TYPE_SHM:
            break;
        case MEM_TYPE_POOL:
        {
            MemBlkInfo *mi = (MemBlkInfo *)minfo;
            int id = mi->pool_index;
            if (id >= MAXMEMPOOL){
                free(addr);
                return;
            }
            MemPool *mp = mpool_slots[id];
            DEBUG("---------mp: %p, mem free: %p", mp, addr);
            mempool_put(mp, addr);
            break;
        }
        default:
            free(addr);
            break;
    }

}

