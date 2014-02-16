#include <stdio.h>

#include "stack.h"
#include "log.h"

int main(void)
{
    Stack *stack = stack_new(25);

    char data[17] = "0123456789ABCDEF";
    char buf[9] = {0};

    stack_push(stack, data, 16);
    DEBUG("----stack push: %s", data);
    stack_pop(stack, buf, 8);
    DEBUG("----stack pop: %s", buf);
    stack_pop(stack, buf, 8);
    DEBUG("----stack pop: %s", buf);

    return 0;
}
