#ifndef FT_FTHREAD_H
#define FT_FTHREAD_H

#include "bits/fthreadtypes.h"

/* thread functions */

int fthread_create(fthread_t *thread, void *(*start_routine)(void *),
                   void *arg);
fthread_t fthread_self();
int fthread_equal(fthread_t t1, fthread_t t2);
void fthread_yield();
void fthread_exit(void *retval);
int fthread_detach(fthread_t thread);
int fthread_join(fthread_t thread, void **retval);

/* mutex functions */

int fthread_mutex_init(fthread_mutex_t *mutex);
int fthread_mutex_lock(fthread_mutex_t *mutex);
int fthread_mutex_trylock(fthread_mutex_t *mutex);
int fthread_mutex_unlock(fthread_mutex_t *mutex);
int fthread_mutex_destroy(fthread_mutex_t *mutex);

/* condition variable functions */

int fthread_cond_init(fthread_cond_t *cond);
int fthread_cond_wait(fthread_cond_t *cond, fthread_mutex_t *mutex);
int fthread_cond_signal(fthread_cond_t *cond);
int fthread_cond_broadcast(fthread_cond_t *cond);
int fthread_cond_destroy(fthread_cond_t *cond);

#endif
