#include <errno.h>
#include "fthread.h"
#include "ft_globals.h"
#include "ft_queue.h"
#include "ft_sched.h"

int fthread_cond_init(fthread_cond_t *cond) {
  cond->__waitm = 0;
  queue_init(&cond->__waitq);
  cond->__usecount = 0;
  return 0;
}

int fthread_cond_wait(fthread_cond_t *cond, fthread_mutex_t *mutex) {
  if (cond->__waitm) {
    /* there should only be one mutex being used for concurrent
       waits on a single condition variable */
    if (cond->__waitm != mutex) {
      return EINVAL;
    }
  } else {
    cond->__waitm = mutex;
  }
  /* we also need to own the mutex */
  int err = fthread_mutex_unlock(mutex);
  if (err) {
    return err;
  }

  /* can't destroy a mutex while it's being used in a cond_wait:
   * have to up the use count so any calls to destroy will fail with
   * EBUSY */
  mutex->__usecount++;
  cond->__usecount++;
  sched_sleep_on(&cond->__waitq);
  fthread_mutex_lock(mutex);
  mutex->__usecount--;
  cond->__usecount--;
  return 0;
}

int fthread_cond_signal(fthread_cond_t *cond) {
  sched_wakeup_on(&cond->__waitq);
  return 0;
}

int fthread_cond_broadcast(fthread_cond_t *cond) {
  sched_broadcast_on(&cond->__waitq);
  return 0;
}

int fthread_cond_destroy(fthread_cond_t *cond) {
  if (cond->__usecount) {
    return EBUSY;
  }

  queue_destroy(&cond->__waitq);
  return 0;
}
