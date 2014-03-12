#include <stdio.h>
#include "log.h"
#include "mem.h"

int main(void)
{
    char *mem = (char *)mem_poolget(100);
    
    sprintf(mem, "%s", "hello world!\n");

    DEBUG("%s", mem);

    return 0;
}
