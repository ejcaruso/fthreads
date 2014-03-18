#include "fthread.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "ft_assert.h"
#include "ft_config.h"
#include "ft_context.h"
#include "ft_globals.h"
#include "ft_queue.h"
#include "ft_sched.h"
#include "ft_types.h"

struct _fthread main_thread;

static queue_t thr_reapq = __QUEUE_INITIALIZER;
static queue_t reapd_waitq = __QUEUE_INITIALIZER;

static void submit_to_reapd(fthread_t thr);
static void *reapd(void *arg);

static int num_threads = 1;

int fthread_create(
  fthread_t *thread,
  void *(*start_routine)(void *),
  void *arg
) {
  // TODO: slab allocation instead
  struct _fthread *thr = (struct _fthread *)malloc(sizeof(struct _fthread));
  if (!thr) {
    return EAGAIN;
  }
  
  // map stack
  long page_size = sysconf(_SC_PAGE_SIZE);
  void *stack_bottom = mmap(0, STACK_SIZE_PAGES * page_size,
                            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
                            -1, 0);
  if (stack_bottom == MAP_FAILED) {
    return EAGAIN;
  }

  // protect bottom of stack
  ftassert(mmap(stack_bottom, page_size, PROT_NONE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0) == stack_bottom);
  
  // initialize thread state
  void *stack_top = stack_bottom + STACK_SIZE_PAGES * page_size;
  context_init(&thr->cont, start_routine, arg, stack_top);
  thr->state = AVAILABLE;
  thr->retval = 0;
  thr->detached = 0;
  thr->reapable = 0;
  queue_init(&thr->joiners);
  thr->waitchan = 0;
  thr->tstack = stack_bottom;
  *thread = thr;
  
  sched_schedule(thr);
  num_threads++;
  return 0;
}

static void fthread_destroy(fthread_t thr) {
  // unmap stack
  long page_size = sysconf(_SC_PAGE_SIZE);
  munmap(thr->tstack, STACK_SIZE_PAGES * page_size);
  
  // clear other thread state
  queue_destroy(&thr->joiners);

  if (thr != &main_thread) {
    // main thread was statically allocated
    free(thr);
  }
}

fthread_t fthread_self() {
  return current_thread;
}

int fthread_equal(fthread_t t1, fthread_t t2) {
  return t1 == t2;
}

void fthread_yield() {
  sched_relinquish();
}

void fthread_exit(void *retval) {
  current_thread->state = EXITED;
  current_thread->retval = retval;
 
  if (current_thread->detached) {
    submit_to_reapd(current_thread);
  } else {
    num_threads--;
  }
  sched_switch();
  ftpanic("Returned to exited thread");
}

int fthread_detach(fthread_t thread) {
  if (thread->detached || !queue_empty(&thread->joiners)) {
    return EINVAL;
  }

  if (thread->state == EXITED) {
    // we can submit to the reapd, but it doesn't matter
    fthread_destroy(thread);
  } else {
    thread->detached = 1;
  }
  return 0;
}

int fthread_join(fthread_t thread, void **retval) {
  if (thread->detached || !queue_empty(&thread->joiners)) {
    return EINVAL;
  }

  // detect join cycles
  fthread_t joining_thread = thread;
  while ((joining_thread = joining_thread->joiners.__head->data)) {
    if (joining_thread == current_thread) {
      return EDEADLK;
    }
  }

  if (thread->state != EXITED) {
    sched_sleep_on(&thread->joiners);
  }

  *retval = thread->retval;
  fthread_destroy(thread);
  return 0;
}

/* REAPD FUNCTIONS BELOW */

static void __attribute__((constructor)) reapd_init() {
  fthread_t reapd_thr;
  fthread_create(&reapd_thr, reapd, 0);
}

static void submit_to_reapd(fthread_t thr) {
  if (thr->reapable) {
    return;
  }
  thr->reapable = 1;
  queue_enqueue(&thr_reapq, thr);
  sched_wakeup_on(&reapd_waitq);
}

static void *reapd(void *arg) {
  for (;;) {
    sched_sleep_on(&reapd_waitq);
    fthread_t dead_thr;
    while ((dead_thr = queue_dequeue(&thr_reapq))) {
      fthread_destroy(dead_thr);
      num_threads--;
    }
    if (num_threads == 1) {
      exit(0);
    }
  }
  return 0;
}
