#include <stdlib.h>

#include "trace.h"

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
    trace_init("./");
    test2();
    return 0;
}
