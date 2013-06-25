#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#include "context.h"

static struct fcontext main_context;
static struct fcontext contexts[2];

static void *context0(void *arg) {
  printf("Hello from context 0\n");
  context_switch(&contexts[0], &contexts[1]);
  printf("Hello again from context 0\n");
  context_switch(&contexts[0], &main_context);
  return 0;
}

static void *context1(void *arg) {
  printf("Hello from context 1\n");
  context_switch(&contexts[1], &contexts[0]);
  return 0;
}

void *make_stack() {
  void *addr = malloc(4096);
  return (void *)((char *)addr + 4096);
}

int main(int argc, char **argv) {
  printf("Context switch test\n");
  context_init(&contexts[0], context0, 0, make_stack());
  context_init(&contexts[1], context1, 0, make_stack());
  context_switch(&main_context, &contexts[0]);
  printf("Success\n");
  return 0;
}
