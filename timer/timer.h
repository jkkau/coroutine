#ifndef CO_TIMER_H
#define CO_TIMER_H

#include "co/co.h"
#include <stdint.h>

#define TIMER_SECOND 1000
#define TIMER_MILLISECOND 1

typedef bool (*is_timer_keep_running)();

void timer_init(is_timer_keep_running r);

// expiryTime的最小单位是10ms
void timer_add(coroutine_t *co, int expiryTime/*millisecond*/);

/*
阻塞函数，必须在主协程运行
*/
void timer_run();

uint64_t get_current_time_ms();

#endif  // CO_TIMER_H