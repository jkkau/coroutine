#include "co.h"
#include "timer/timer.h"
#include <stdlib.h>
#include <stdint.h>

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
	current->finished = true;
	_swap(&main_routine.ctx, &current->ctx);
}

static void _co_build_stack(coroutine_t *co)
{
	uintptr_t top = (uintptr_t)(co->stack + STACK_SIZE - sizeof(void *));
	// 让top变为可被0xF整除，达到rsp是16位字节对齐的目的
	top = top & ~0xF;
	*(void **)top = (void *)_co_entry;
	co->ctx.rsp = (void *)top;
	co->ctx.rbp = co->stack;
}

void co_init(coroutine_t *co, co_func f)
{
	posix_memalign(&co->stack, 16, STACK_SIZE);
	_co_build_stack(co);
	co->func = f;
	co->finished = false;
}

/*
作用: 继续运行协程co
只能在main函数对应的协程中调用，应该该函数会将当前的栈信息保存到main_routine中
*/
void co_resume(coroutine_t *co)
{
	if (co->finished) {
		_co_build_stack(co);
		co->finished = false;
	}
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
		co->stack = NULL;
	}
}

void co_sleep(int ms)
{
	timer_add(current, ms);
	co_yield();
}
