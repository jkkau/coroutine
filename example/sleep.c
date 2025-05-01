#include <stdio.h>
#include "co/co.h"
#include "timer/timer.h"

coroutine_t coroutines[2];

void func0()
{
	printf("func0 step1 at %ld ms\n", get_current_time_ms());
	co_sleep(TIMER_SECOND);
	printf("func0 step2 at %ld ms\n", get_current_time_ms());
}

void func1()
{
	printf("func1 step1 at %ld ms\n", get_current_time_ms());
	co_sleep(10 * TIMER_MILLISECOND);
	printf("func1 step2 at %ld ms\n", get_current_time_ms());
}

bool running()
{
	return !(coroutines[0].finished && coroutines[1].finished);
}

int main()
{
	timer_init(running);

	co_init(&coroutines[0], func0);
	co_init(&coroutines[1], func1);

	co_resume(&coroutines[0]);
	co_resume(&coroutines[1]);

	timer_run();

	// co_destroy(&coroutines[0]);
	// co_destroy(&coroutines[1]);

	return 0;
}