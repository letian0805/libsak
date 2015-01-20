#include <stdio.h>
#include "sak.h"
#include "mem.h"

int main(void)
{
    char *mem = (char *)mem_poolget(100);
    
    sprintf(mem, "%s", "hello world!\n");

    SAK_DEBUG("%s", mem);

    return 0;
}
