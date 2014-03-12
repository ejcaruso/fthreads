#include <errno.h>
#include "fthread.h"
#include "ft_globals.h"
#include "ft_queue.h"
#include "ft_sched.h"

int fthread_barrier_init(fthread_barrier_t *barrier, unsigned int count) {
  queue_init(&barrier->__waitq);
  barrier->__waiting = 0;
  barrier->__max = count;
  return 0;
}

int fthread_barrier_wait(fthread_barrier_t *barrier) {
  if (++barrier->__waiting == barrier->__max) {
    barrier->__waiting = 0;
    sched_broadcast_on(&barrier->__waitq);
    return FTHREAD_BARRIER_SERIAL_THREAD;
  }

  sched_sleep_on(&barrier->__waitq);
  return 0;
}

int fthread_barrier_destroy(fthread_barrier_t *barrier) {
  if (barrier->__waiting) {
    return EBUSY;
  }

  queue_destroy(&barrier->__waitq);
  return 0;
}
