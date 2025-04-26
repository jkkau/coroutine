#include <stdio.h>
#include <stdlib.h>

typedef void (*co_func)();

typedef struct {
    void *rsp;
    void *rbp;
    // void *eip;
}co_context_t;

#define STACK_SIZE 2 * 1024
typedef struct {
    co_context_t ctx;
    void *stack;
}coroutine_t;

extern int swap(co_context_t *next_co, co_context_t *cur_co);

void coroutine_init(coroutine_t *co, co_func f) {
    co->stack = malloc(STACK_SIZE);
    *(void **)(co->stack + STACK_SIZE - sizeof(void *)) = (void *)f;
    co->ctx.rsp = co->stack + STACK_SIZE - sizeof(void *);
    co->ctx.rbp = NULL;
}

coroutine_t coroutines[2];
coroutine_t main_routine;

void func0() {
    printf("func0\n");
    swap(&main_routine.ctx, &coroutines[0].ctx);

    __builtin_unreachable();
}

void func1() {
    printf("func1\n");
    swap(&main_routine.ctx, &coroutines[1].ctx);

    __builtin_unreachable();
}

int main() {
    coroutine_init(&coroutines[0], func0);
    coroutine_init(&coroutines[1], func1);

    printf("start\n");
    swap(&coroutines[0].ctx, &main_routine.ctx);
    printf("fun0 finished.\n");
    swap(&coroutines[1].ctx, &main_routine.ctx);
    printf("func1 finished.\n");

    return 0;
}