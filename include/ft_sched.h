#ifndef FT_SCHED_H
#define FT_SCHED_H

#include "fthread.h"
#include "ft_queue.h"

void      sched_switch();
void      sched_schedule(fthread_t thr);
void      sched_relinquish();
void      sched_sleep_on(queue_t *q);
fthread_t sched_wakeup_on(queue_t *q);
void      sched_broadcast_on(queue_t *q);

#endif
