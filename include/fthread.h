#ifndef FT_FTHREAD_H
#define FT_FTHREAD_H

struct _fthread;
typedef struct _fthread *fthread_t;

int fthread_create(fthread_t *thread, void *(*start_routine)(void *),
                   void *arg);
fthread_t fthread_self();
int fthread_equal(fthread_t t1, fthread_t t2);
void fthread_yield();
void fthread_exit(void *retval);

struct _fmutex;
typedef struct _fmutex *fthread_mutex_t;

int fthread_mutex_init(fthread_mutex_t *mutex);
int fthread_mutex_lock(fthread_mutex_t *mutex);
int fthread_mutex_trylock(fthread_mutex_t *mutex);
int fthread_mutex_unlock(fthread_mutex_t *mutex);
int fthread_mutex_destroy(fthread_mutex_t *mutex);

#endif
