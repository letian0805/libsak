#include <stdio.h>
#include "sak.h"

#include "queue.h"

int main(void)
{
    Queue *queue = queue_new(25);

    char data[17] = "0123456789ABCDEF";
    char buf[9] = {0};

    queue_put(queue, data, 16);
    SAK_DEBUG("----queue put: %s", data);
    queue_get(queue, buf, 8);
    SAK_DEBUG("----queue get: %s", buf);
    queue_put(queue, data, 16);
    SAK_DEBUG("----queue put: %s", data);
    queue_get(queue, buf, 8);
    SAK_DEBUG("----queue get: %s", buf);
    queue_enlarge(queue, 1.5);
    queue_get(queue, buf, 8);
    SAK_DEBUG("----queue get: %s", buf);
    queue_get(queue, buf, 8);
    SAK_DEBUG("----queue get: %s", buf);

    return 0;
}
