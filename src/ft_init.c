#include "ft_globals.h"
#include "ft_types.h"

extern struct _fthread main_thread;

static void __attribute__((constructor)) fthreads_init() {
  current_thread = &main_thread;
}
