#include <stdio.h>
#include <string.h>

#include "mempool.h"
#include "mem.h"
#include "log.h"

int main(void)
{

    MemPool *mp = mempool_new(64);
    uint8_t *mem1 = mempool_get(mp);
    strcpy(mem1, "abcdefg");
    mempool_put(mp, mem1);
    uint8_t *mem2 = mempool_get(mp);
    DEBUG("---------mem2: %s\n", mem2);

    void *mem = mem_poolget(28);
    strcpy(mem, "hello world!");
    DEBUG("--------mem:%p\n", mem);
    mem_free(mem);
    mem = mem_poolget(28);
    DEBUG("--------mem:%p %s\n",mem, mem);

    return 0;
}
