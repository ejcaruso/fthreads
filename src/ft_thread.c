#include "fthread.h"

#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "ft_config.h"
#include "ft_context.h"
#include "ft_globals.h"
#include "ft_queue.h"
#include "ft_sched.h"
#include "ft_types.h"

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
  
  // allocate stack
  long page_size = sysconf(_SC_PAGE_SIZE);
  void *stack_bottom = mmap(0, STACK_SIZE_PAGES * page_size,
                            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
                            -1, 0);
  if (stack_bottom == MAP_FAILED) {
    return EAGAIN;
  }
  // protect bottom of stack
  if (mmap(stack_bottom, page_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS,
           -1, 0) == MAP_FAILED) {
    // should never actually happen
    return EAGAIN;
  }
  
  // initialize thread state
  void *stack_top = stack_bottom + STACK_SIZE_PAGES * page_size;
  context_init(&thr->cont, start_routine, arg, stack_top);
  thr->state = AVAILABLE;
  thr->retval = 0;
  thr->waitchan = 0;
  thr->tstack = stack_bottom;
  *thread = thr;
  
  sched_schedule(thr);
  return 0;
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
  sched_switch();
}
