#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sak.h"

#include "mempool.h"
#include "mem.h"

#define BLKSIZE (1024)
#define TESTSIZE (1024*1024*1024ull)

int main(int argc, char *argv[])
{
    int type = 0;
    if (argc == 2){
        type = atoi(argv[1]);
    }
    MemPool *mp = mempool_new(BLKSIZE);
    void *mem1 = NULL;

    int i;
    if (type == 0){
        for (i = 0; i < TESTSIZE/BLKSIZE; i++){
            mem1 = mempool_get(mp);
        }
        uint64_t size = mempool_memsize(mp);
        printf("------memsize: %llu, oobsize: %llu, oobrate: %f\n", (ull)size, (ull)size - TESTSIZE, (size - TESTSIZE)*1.0/TESTSIZE);
    }else{
        for (i = 0; i < TESTSIZE/BLKSIZE; i++){
            mem1 = malloc(BLKSIZE);
        }
    }

    return 0;
}
