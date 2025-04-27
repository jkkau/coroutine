#include <stdio.h>
#include "co.h"

coroutine_t coroutines[2];

void func0()
{
	printf("func0 step1\n");
	co_yield();
	printf("func0 step2\n");
}

void func1()
{
	printf("func1 step1\n");
	co_yield();
	printf("func1 step2\n");
}

int main()
{
	co_init(&coroutines[0], func0);
	co_init(&coroutines[1], func1);

	printf("start\n");
	co_resume(&coroutines[0]);
	co_resume(&coroutines[1]);
	co_resume(&coroutines[0]);
	printf("fun0 finished.\n");
	co_resume(&coroutines[1]);
	printf("func1 finished.\n");

	printf("again\n");
	co_resume(&coroutines[0]);
	co_resume(&coroutines[1]);
	co_resume(&coroutines[0]);
	printf("fun0 finished.\n");
	co_resume(&coroutines[1]);
	printf("func1 finished.\n");

	co_destroy(&coroutines[0]);
	co_destroy(&coroutines[1]);

	return 0;
}