#ifndef _ULOOP_TIMER_H__
#define _ULOOP_TIMER_H__
#include <sys/time.h> 
#include "list.h"

struct ecnt_uloop_timeout;

typedef void (*ecnt_uloop_timeout_handler)(struct ecnt_uloop_timeout *t);

struct ecnt_uloop_timeout
{
	struct list_head list;
	bool pending;

	ecnt_uloop_timeout_handler cb;
	struct timeval time;
};

int ecnt_uloop_timeout_add(struct ecnt_uloop_timeout *timeout);
int ecnt_uloop_timeout_set(struct ecnt_uloop_timeout *timeout, int msecs);
int ecnt_uloop_timeout_cancel(struct ecnt_uloop_timeout *timeout);
int ecnt_uloop_timeout_remaining(struct ecnt_uloop_timeout *timeout);
int ecnt_uloop_run(void);


#endif

