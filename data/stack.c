#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "stack.h"

struct Stack{
    int size;
    int top;
    uint8_t *st;
};

Stack *stack_new(int size)
{
    Stack *st = (Stack *)malloc(sizeof(Stack));
    if (!st){
        return NULL;
    }

    st->st = (uint8_t *)malloc(size);
    if (!st->st){
        free(st);
        return NULL;
    }
    st->size = size;
    st->top = 0;

    return st;
}

bool stack_full(Stack *st)
{
    return (st->top > st->size);
}

bool stack_empty(Stack *st)
{
    return (st->top == 0);
}

int stack_enlarge(Stack *st, float rate)
{
    int new_size = st->size * rate;
    if (new_size <= st->size){
        return -1;
    }

    uint8_t *new_stack = (uint8_t *)realloc(st->st, new_size);
    if (new_stack){
        st->st = new_stack;
        st->size = new_size;
    }

    return 0;
}

int stack_push(Stack *st, void *data, int size)
{
    if (stack_full(st) || st->size - st->top < size){
        return -1;
    }

    memcpy(st->st + st->top, data, size);

    st->top += size;

    return 0;
}

int stack_pop(Stack *st, void *dst, int size)
{
    if (stack_empty(st) || st->top < size){
        return -1;
    }
    st->top -= size;
    memcpy(dst, st->st + st->top, size);

    return 0;
}

int stack_size(Stack *st)
{
    return st->size;
}

void stack_free(Stack *st)
{
    free(st->st);
    free(st);
}

