#include <errno.h>
#include "fthread.h"
#include "ft_globals.h"
#include "ft_queue.h"
#include "ft_sched.h"

int fthread_mutex_init(fthread_mutex_t *mutex) {
  mutex->__holder = 0;
  queue_init(&mutex->__waitq);
  mutex->__usecount = 0;
  return 0;
}

int fthread_mutex_lock(fthread_mutex_t *mutex) {
  mutex->__usecount++;
  if (mutex->__holder) {
    if (mutex->__holder == current_thread) {
      mutex->__usecount--;
      return EDEADLK;
    }
    sched_sleep_on(&mutex->__waitq);
  }

  mutex->__holder = current_thread;
  return 0;
}

int fthread_mutex_trylock(fthread_mutex_t *mutex) {
  if (mutex->__holder) {
    return EBUSY;
  }

  mutex->__usecount++;
  mutex->__holder = current_thread;
  return 0;
}

int fthread_mutex_unlock(fthread_mutex_t *mutex) {
  if (mutex->__holder != current_thread) {
    return EPERM;
  }

  mutex->__holder = sched_wakeup_on(&mutex->__waitq);
  mutex->__usecount--;
  return 0;
}

int fthread_mutex_destroy(fthread_mutex_t *mutex) {
  if (mutex->__usecount) {
    return EBUSY;
  }

  queue_destroy(&mutex->__waitq);
  return 0;
}
