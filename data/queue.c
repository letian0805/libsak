#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "queue.h"

struct Queue{
    int size;
    int count;
    int head;
    int tail;
    uint8_t *q;
};

Queue *queue_new(int size)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (queue){
        queue->q = (uint8_t *)malloc(size);
        if (!queue->q){
            free(queue);
            return NULL;
        }
        queue->size = size;
        queue->count = 0;
        queue->head = 0;
        queue->tail = 0;
    }

    return queue;
}

int queue_enlarge(Queue *q, float rate)
{
    if (rate <= 1){
        rate = 1.5;
    }
    int new_size = (int)(q->size * rate);
    uint8_t *new_buf = (uint8_t *)realloc(q->q, new_size);
    if (!new_buf){
        return -1;
    }

    q->q = new_buf;
    if (q->count == q->size && q->tail == 0){
        q->tail = q->size;
    }else if (q->count > 0 && q->head >= q->tail) {
        int size = q->size - q->head;
        memmove(q->q + new_size - size, q->q + q->head, size);
        q->head = new_size - size;
    }
    q->size = new_size;
    
    return 0;
}

bool queue_full(Queue *q)
{
    return (q->count == q->size);
}

bool queue_empty(Queue *q)
{
    return (q->count == 0);
}

int queue_put(Queue *q, void *data, int size)
{
    if (q->size - q->count < size){
        return -1;
    }

    int first_part = q->size - q->tail;
    if (first_part > size){
        first_part = size;
    }
    int second_part = size - first_part;
    if (first_part){
        memcpy(q->q + q->tail, data, first_part);
        q->tail += first_part;
    }
    if (second_part){
        memcpy(q->q, (uint8_t *)data + first_part, second_part);
        q->tail = second_part;
    }

    q->count += size;

    return 0;
}

int queue_get(Queue *q, void *dst, int size)
{
    if (q->count < size){
        return -1;
    }

    int first_part = q->size - q->head;
    if (first_part > size){
        first_part = size;
    }
    int second_part = size - first_part;
    if (first_part){
        memcpy(dst, q->q + q->head, first_part);
        q->head += first_part;
    }
    if (second_part){
        memcpy((uint8_t *)dst + first_part, q->q, second_part);
        q->head = second_part;
    }
    q->count -= size;

    return 0;
}

void queue_free(Queue *q)
{
    free(q->q);
    free(q);
}
