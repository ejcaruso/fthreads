#ifndef FT_FTHREAD_STRUCT_H
#define FT_FTHREAD_STRUCT_H

#include "ft_context.h"
#include "ft_queue.h"

typedef enum fstate {
  NO_STATE,
  RUNNING,
  AVAILABLE,
  SLEEPING,
  EXITED
} fstate_t;

struct _fthread {
  // various pieces of thread state
  fcontext_t  cont;
  fstate_t    state;
  void       *retval;
  int         terrno;
  // join/detach information
  int         detached;
  int         reapable;
  queue_t     joiners;
  // current wait queue, or runqueue
  queue_t    *waitchan;
  // thread stack
  void       *tstack;
};

#endif
