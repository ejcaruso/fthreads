#ifndef FTHREADS_CONTEXT_H
#define FTHREADS_CONTEXT_H

typedef struct fcontext {
  void *ebp;
  void *esp;
} fcontext_t;

void context_init(
  fcontext_t *cont,
  void *(*func)(void *),
  void *arg,
  void *stack_top
);

void context_switch(fcontext_t *cont, fcontext_t *next);

#endif
