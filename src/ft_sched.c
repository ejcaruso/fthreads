#include "ft_sched.h"

#include <sched.h>

#include "fthread.h"
#include "ft_context.h"
#include "ft_globals.h"
#include "ft_queue.h"
#include "ft_types.h"

static queue_t sched_runq = __QUEUE_INITIALIZER;

void __attribute__((constructor)) sched_init() {
  // right now, this is a no-op. this could do more work if we need
  // a more complicated scheduler, for instance
}

void sched_switch() {
  fthread_t next_thread;
  while (!(next_thread = (fthread_t)queue_dequeue(&sched_runq))) {
    // TODO: select across pending I/O requests
    for (;;) sched_yield();
  }

  fthread_t prev_thread = current_thread;
  current_thread = next_thread;
  current_thread->state = RUNNING;
  
  context_switch(&prev_thread->cont, &current_thread->cont);
  // now we're in the context of the next thread
}

void sched_schedule(fthread_t thr) {
  queue_enqueue(&sched_runq, thr);
}

void sched_relinquish() {
  current_thread->state = AVAILABLE;
  sched_schedule(current_thread);
  sched_switch();
}

void sched_sleep_on(queue_t *q) {
  current_thread->state = SLEEPING;
  current_thread->waitchan = q;
  queue_enqueue(q, current_thread);
  sched_switch();
}

fthread_t sched_wakeup_on(queue_t *q) {
  fthread_t thr = (fthread_t)queue_dequeue(q);
  if (!thr) {
    return 0;
  }
  
  thr->state = AVAILABLE;
  queue_enqueue(&sched_runq, thr);
  return thr;
}

void sched_broadcast_on(queue_t *q) {
  while (sched_wakeup_on(q));
}
