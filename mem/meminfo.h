#ifndef __MEMINFO_H
#define __MEMINFO_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MEM_MAGIC 0xCAFE

#define MAXMEMPOOL ((1<<10)-1)

typedef struct{
    uint64_t magic:16;
    uint64_t type:8;
    uint64_t pool_index:10;
    uint64_t chunk_index:14;
    uint64_t block_index:16;
}MemBlkInfo;

typedef struct{
    uint64_t magic:16;
    uint64_t type:8;
    uint64_t size:40;
}MemInfo;

typedef struct{
    uint64_t magic:16;
    uint64_t type:8;

}MemShmInfo;

typedef struct{
    uint64_t magic:16;
    uint64_t type:8;

}MemMapInfo;

typedef enum{
    BLK_TYPE_MALLOC = 0,
    BLK_TYPE_MMAP,
    BLK_TYPE_SHM,
    BLK_TYPE_POOL,

    BLK_TYPE_LAST,
}MemBlkType;

int mempool_make_blksize(int size);

static inline void meminfo_init(int type, MemInfo *minfo)
{
    minfo->type = type;
    minfo->magic = MEM_MAGIC;
}

static inline bool meminfo_check(MemInfo *minfo)
{
    return (minfo->magic == MEM_MAGIC && minfo->type < BLK_TYPE_LAST);
}
#endif //!__MEMINFO_H
