#include <stdlib.h>

#include "sak.h"

void test1(void)
{
    char *a = NULL;
    *a = 1;
}

void test2(void)
{
    test1();
}

int main(void)
{
    sak_trace_init("./");
    test2();
    return 0;
}
