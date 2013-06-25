#include "context.h"

void trampoline(void *(*func)(void *), void *arg);

void context_init(
  fcontext_t *cont,
  void *(*func)(void *),
  void *arg,
  void *stack_top
) {
  void **stack_ptr = (void **)stack_top;

  // arguments to trampoline
  *(--stack_ptr) = arg;        // argument 2
  *(--stack_ptr) = func;       // argument 1
  // partial stack frame for trampoline
  *(--stack_ptr) = 0;          // dummy return address
  void *old_ebp = stack_ptr;

  // arguments to context_switch
  *(--stack_ptr) = 0;          // dummy argument 2
  *(--stack_ptr) = cont;       // argument 1
  // stack frame for context_switch
  *(--stack_ptr) = trampoline; // return address
  *(--stack_ptr) = old_ebp;    // old ebp
  void *new_ebp = stack_ptr;
  *(--stack_ptr) = 0;          // dummy saved ebx
  *(--stack_ptr) = 0;          // dummy saved esi
  *(--stack_ptr) = 0;          // dummy saved edi

  // save stack state into context
  cont->ebp = new_ebp;
  cont->esp = stack_ptr;
}
