#include "co.h"
#include <stdlib.h>

extern int _swap(co_context_t *next_co, co_context_t *cur_co);

// 当前正在执行的coroutine
coroutine_t *current = NULL;

// main函数对应的coroutine
coroutine_t main_routine;

/*
_co_entry是所有协程运行的入口函数
通过该函数来统一管理协程，当co_func执行结束后，swap回主协程
*/
static void _co_entry()
{
	if (current && current->func) {
		current->func();
	}
	_swap(&main_routine.ctx, &current->ctx);
}

void co_init(coroutine_t *co, co_func f)
{
	posix_memalign(&co->stack, 16, STACK_SIZE);
	*(void **)(co->stack + STACK_SIZE - sizeof(void *)) = (void *)_co_entry;
	co->ctx.rsp = co->stack + STACK_SIZE - sizeof(void *);
	co->ctx.rbp = NULL;
	co->func = f;
}

/*
作用: 继续运行协程co
只能在main函数对应的协程中调用，应该该函数会将当前的栈信息保存到main_routine中
*/
void co_resume(coroutine_t *co)
{
	current = co;
	_swap(&current->ctx, &main_routine.ctx);
}

// 切换回主协程
void co_yield()
{
	_swap(&main_routine.ctx, &current->ctx);
}

void co_destroy(coroutine_t *co)
{
	if (co->stack != NULL) {
		free(co->stack);
	}
}