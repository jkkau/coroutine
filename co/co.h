#ifndef CO_H
#define CO_H

#include <stdbool.h>

typedef void (*co_func)();

typedef struct {
	void *rsp;
	void *rbp;
	// void *eip;
} co_context_t;

#define STACK_SIZE 10 * 1024
typedef struct {
	co_context_t ctx;
	void *stack;
	co_func func;
	bool finished;
} coroutine_t;

void co_init(coroutine_t *co, co_func f);
void co_resume(coroutine_t *co);
void co_yield();
void co_destroy(coroutine_t *co);
void co_sleep(int ms);

// extern int swap(co_context_t *next_co, co_context_t *cur_co);
extern coroutine_t main_routine;

#endif  // CO_H