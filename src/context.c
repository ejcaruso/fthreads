#include "context.h"

void trampoline(void *(*func)(void *), void *arg);

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
  // partial stack frame for trampoline
  PUSH(0);          // dummy return address
  void *old_ebp = stack_ptr;

  // arguments to context_switch
  PUSH(0);          // dummy argument 2
  PUSH(cont);       // argument 1
  // stack frame for context_switch
  PUSH(trampoline); // return address
  PUSH(old_ebp);    // old ebp
  void *new_ebp = stack_ptr;
  PUSH(0);          // dummy saved ebx
  PUSH(0);          // dummy saved esi
  PUSH(0);          // dummy saved edi

  // save stack state into context
  cont->ebp = new_ebp;
  cont->esp = stack_ptr;
}
