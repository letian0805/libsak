#include <stdio.h>

#include "stack.h"
#include "queue.h"
#include "log.h"
#include "mempool.h"

int main(void)
{
    Stack *stack = stack_new(25);
    Queue *queue = queue_new(25);

    char data[17] = "0123456789ABCDEF";
    char buf[9] = {0};

    stack_push(stack, data, 16);
    DEBUG("----stack push: %s", data);
    stack_pop(stack, buf, 8);
    DEBUG("----stack pop: %s", buf);
    stack_pop(stack, buf, 8);
    DEBUG("----stack pop: %s", buf);

    queue_put(queue, data, 16);
    DEBUG("----queue put: %s", data);
    queue_get(queue, buf, 8);
    DEBUG("----queue get: %s", buf);
    queue_put(queue, data, 16);
    DEBUG("----queue put: %s", data);
    queue_get(queue, buf, 8);
    DEBUG("----queue get: %s", buf);
    queue_get(queue, buf, 8);
    DEBUG("----queue get: %s", buf);
    queue_get(queue, buf, 8);
    DEBUG("----queue get: %s", buf);

    MemPool *mp = mempool_new(50, 20*1024);

    void *mem = mempool_get(mp);

    return 0;
}
