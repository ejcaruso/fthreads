#include "ft_context.h"

#include "fthread.h"

static void trampoline(void *(*func)(void *), void *arg) {
  fthread_exit(func(arg));
}

void context_init(
  fcontext_t *cont,
  void *(*func)(void *),
  void *arg,
  void *stack_top
) {
  void **stack_ptr = (void **)stack_top;
#define PUSH(arg) *(--stack_ptr) = (arg);

  // arguments to trampoline
  PUSH(arg);        // argument 2
  PUSH(func);       // argument 1
  PUSH(0);          // dummy return value
  void *old_ebp = stack_ptr;

  // stack frame for context_switch
  // no arguments because they don't matter...
  PUSH(trampoline); // first function
  PUSH(old_ebp);    // old ebp
  void *new_ebp = stack_ptr;
  PUSH(0);            // dummy saved ebx
  PUSH(0);            // dummy saved esi
  PUSH(0);            // dummy saved edi

  // save stack state into context
  cont->ebp = new_ebp;
  cont->esp = stack_ptr;
}
