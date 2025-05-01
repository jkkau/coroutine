#include "timer.h"
#include <stdio.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

#define TIME_WHEEL_SLOT_NUMBER 100
// 每隔10ms时间轮转动一格
#define TICK_DURATION 10

typedef struct timer_node {
	coroutine_t *co;
	struct timer_node *p_next;
	uint64_t expiryTime;
} timer_node_t;

/*
时间轮由MAX_TIMER_NODE_NUMBER个格子组成，
每个格子中存储多个timer_node(链表形式储存),
指针每个TICK_DURATION时间移动一格
*/
typedef struct {
	timer_node_t* slots[TIME_WHEEL_SLOT_NUMBER];
	// 时间轮指针当前所在的位置
	int current_pos;
} time_wheel_t;

static time_wheel_t g_tw;
static is_timer_keep_running running;

/*
https://stackoverflow.com/questions/3756323/how-to-get-the-current-time-in-milliseconds-from-c-in-linux
*/
uint64_t get_current_time_ms()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	// ts.tv_nsec+500000相当于round(ts.tv_nsec)
	// 假设tv_nsec是1500000, 我们认为它是2ms而不是1ms
	// 而对于tv_nsec=1200000，我们认为它是1ms
	return (ts.tv_sec * 1000) + ((ts.tv_nsec + 500000)/ 1000000);
}

void timer_init(is_timer_keep_running r)
{
	for (int i = 0; i < TIME_WHEEL_SLOT_NUMBER; i++) {
		g_tw.slots[i] = NULL;
	}

	g_tw.current_pos = 0;
	running = r;
}

void timer_add(coroutine_t *co, int expiryTime)
{
	int slot = expiryTime/TICK_DURATION;
	if (slot == 0) slot = 1;
	int nextPos = (g_tw.current_pos+slot)%TIME_WHEEL_SLOT_NUMBER;

	// 新增节点插入到链表头部，相比于每次插入尾部减少了遍历链表的时间。
	timer_node_t *node = (timer_node_t *)malloc(sizeof(timer_node_t));
	node->co = co;
	node->p_next = g_tw.slots[nextPos];
	node->expiryTime = get_current_time_ms() + expiryTime;
	g_tw.slots[nextPos] = node;
}

void timer_tick()
{
	g_tw.current_pos = (g_tw.current_pos+1)%TIME_WHEEL_SLOT_NUMBER;
	timer_node_t *current = g_tw.slots[g_tw.current_pos];
	timer_node_t *prev = NULL;

	while (current != NULL) {
		uint64_t current_time = get_current_time_ms();
		if (current->expiryTime <= current_time) {
			// timeout
			if (prev) {
				prev->p_next = current->p_next;
			} else {
				g_tw.slots[g_tw.current_pos] = NULL;
			}
			coroutine_t *co = current->co;
			timer_node_t *tmp = current;
			current = current->p_next;
			free(tmp);

			co_resume(co);
		} else {
			prev = current;
			current = current->p_next;
		}
	}
}

void timer_run()
{
	int epfd = epoll_create1(0);
	if (epfd < 0) {
		perror("epoll create failed");
		return;
	}

	struct epoll_event events[1];
	while (running()) {
		int n = epoll_wait(epfd, events, 1, TICK_DURATION);
		if (n == 0) {
			timer_tick();
		}
	}
}