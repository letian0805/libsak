#include <stdio.h>
#include <string.h>

#include "mempool.h"
#include "log.h"

int main(void)
{

    MemPool *mp = mempool_new(64, 20*1024);
    uint8_t *mem1 = mempool_get(mp);
    strcpy(mem1, "abcdefg");
    mempool_put(mp, mem1);
    uint8_t *mem2 = mempool_get(mp);
    DEBUG("---------mem2: %s\n", mem2);

    struct{
        uint32_t a:12;
        uint32_t c:12;
        uint32_t b:4;
        uint32_t d:4;
    }test;
    test.a = 0x123;
    test.b = 0x4;
    test.c = 0x567;
    test.d = 0x8;
    int *p = (int *)&test;
    DEBUG("---------sizeof(test):%d, p: 0x%x\n", sizeof(test), *p);

    return 0;
}
