#ifndef __QUEUE_H
#define __QUEUE_H
#include <stdbool.h>

typedef struct Queue Queue;

Queue *queue_new(int size);

int queue_enlarge(Queue *q, float rate);

bool queue_full(Queue *q);

bool queue_empty(Queue *q);

int queue_put(Queue *q, void *data, int size);

int queue_get(Queue *q, void *dst, int size);

void queue_free(Queue *q);

#endif //!__QUEUE_H
