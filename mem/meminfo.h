#ifndef __MEMINFO_H
#define __MEMINFO_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MEM_TYPE_SHM ('m' << 8 | 'S')
#define MEM_TYPE_MAP ('m' << 8 | 'M')
#define MEM_TYPE_ALC ('m' << 8 | 'A')
#define MEM_TYPE_POOL ('m' << 8 | 'P')
#define MEM_TYPE_CHECK(_info) ( (_info)->type == MEM_TYPE_SHM || \
                                (_info)->type == MEM_TYPE_MAP || \
                                (_info)->type == MEM_TYPE_ALC || \
                                (_info)->type == MEM_TYPE_POOL  \
                                )

#define MAXMEMPOOL ((1<<16)-1)

typedef struct{
    uint64_t type:16;
    uint64_t pool_index:16;
    uint64_t chunk_index:16;
    uint64_t block_index:16;
}MemBlkInfo;

typedef struct{
    uint64_t type:16;
    uint64_t size:48;
}MemInfo;

typedef struct{
    uint64_t type:16;

    uint64_t size:48;
}MemShmInfo;

typedef struct{
    uint64_t type:16;

    uint64_t size:48;
}MemMapInfo;

int mempool_make_blksize(int size);

#endif //!__MEMINFO_H
