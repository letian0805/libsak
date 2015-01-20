#include <stdio.h>
#include "sak.h"

int main(void)
{
    int num = 0;
    SET_BITS(num, 1, 8);
    printf("%x\n", num);
    CLEAR_BITS(num, 2, 5);
    printf("%x\n", num);

    return 0;
}
