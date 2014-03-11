#include <errno.h>
#include <stdlib.h>
#include "fthread.h"
#include "ft_globals.h"
#include "ft_queue.h"
#include "ft_sched.h"
#include "ft_types.h"

int fthread_mutex_init(fthread_mutex_t *mutex) {
  fthread_mutex_t m = (fthread_mutex_t)malloc(sizeof(fthread_mutex_t));
  if (!m) {
    return ENOMEM;
  }

  m->holder = 0;
  queue_init(&m->waitq);
  *mutex = m;
  return 0;
}

int fthread_mutex_lock(fthread_mutex_t *mutex) {
  fthread_mutex_t m = *mutex;
  if (m->holder) {
    if (m->holder == current_thread) {
      return EDEADLK;
    }
    sched_sleep_on(&m->waitq);
  }

  m->holder = current_thread;
  return 0;
}

int fthread_mutex_trylock(fthread_mutex_t *mutex) {
  fthread_mutex_t m = *mutex;
  if (m->holder) {
    return EBUSY;
  }

  m->holder = current_thread;
  return 0;
}

int fthread_mutex_unlock(fthread_mutex_t *mutex) {
  fthread_mutex_t m = *mutex;
  if (m->holder != current_thread) {
    return EPERM;
  }

  m->holder = sched_wakeup_on(&m->waitq);
  return 0;
}

int fthread_mutex_destroy(fthread_mutex_t *mutex) {
  fthread_mutex_t m = *mutex;
  if (m->holder) {
    return EBUSY;
  }

  queue_destroy(&m->waitq);
  free(m);
  return 0;
}
