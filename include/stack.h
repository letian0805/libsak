#ifndef __STACK_H
#define __STACK_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Stack Stack;

Stack *stack_new(int size);

int stack_enlarge(Stack *st, float rate);

bool stack_full(Stack *st);

bool stack_empty(Stack *st);

int stack_push(Stack *st, void *data, int size);

int stack_pop(Stack *st, void *dst, int size);

int stack_size(Stack *st);

void stack_free(Stack *st);

#ifdef __cplusplus
}
#endif

#endif //!__STACK_H
