/*************************
	author: jw.ren
	date:    20190115
*************************/
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "uloop_timer.h"
#include <time.h>


static struct list_head timeouts = LIST_HEAD_INIT(timeouts);

static int ecnt_tv_diff(struct timeval *t1, struct timeval *t2)
{
	return
		(t1->tv_sec - t2->tv_sec) * 1000 +
		(t1->tv_usec - t2->tv_usec) / 1000;
}

int ecnt_uloop_timeout_add(struct ecnt_uloop_timeout *timeout)
{
	struct ecnt_uloop_timeout *tmp;
	struct list_head *h = &timeouts;

	if (timeout->pending)
		return -1;

	list_for_each_entry(tmp, &timeouts, list) {
		if (ecnt_tv_diff(&tmp->time, &timeout->time) > 0) {
			h = &tmp->list;
			break;
		}
	}

	list_add_tail(&timeout->list, h);
	timeout->pending = true;

	return 0;
}

static void ecnt_uloop_gettime(struct timeval *tv)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	tv->tv_sec = ts.tv_sec;
	tv->tv_usec = ts.tv_nsec / 1000;
}

int ecnt_uloop_timeout_set(struct ecnt_uloop_timeout *timeout, int msecs)
{
	struct timeval *time = &timeout->time;

	if (timeout->pending)
		ecnt_uloop_timeout_cancel(timeout);

	ecnt_uloop_gettime(time);

	time->tv_sec += msecs / 1000;
	time->tv_usec += (msecs % 1000) * 1000;

	if (time->tv_usec > 1000000) {
		time->tv_sec++;
		time->tv_usec -= 1000000;
	}

	return ecnt_uloop_timeout_add(timeout);
}

int ecnt_uloop_timeout_cancel(struct ecnt_uloop_timeout *timeout)
{
	if (!timeout->pending)
		return -1;

	list_del(&timeout->list);
	timeout->pending = false;

	return 0;
}

static void ecnt_uloop_process_timeouts(struct timeval *tv)
{
	struct ecnt_uloop_timeout *t;

	while (!list_empty(&timeouts)) {
		t = list_first_entry(&timeouts, struct ecnt_uloop_timeout, list);

		if (ecnt_tv_diff(&t->time, tv) > 0)
			break;

		ecnt_uloop_timeout_cancel(t);
		if (t->cb)
			t->cb(t);
	}
}

static void ecnt_uloop_clear_timeouts(void)
{
	struct ecnt_uloop_timeout *t, *tmp;

	list_for_each_entry_safe(t, tmp, &timeouts, list)
		ecnt_uloop_timeout_cancel(t);
}

int ecnt_uloop_run(void)
{
	struct timeval tv;

	ecnt_uloop_gettime(&tv);
	ecnt_uloop_process_timeouts(&tv);

	return 0;
}


