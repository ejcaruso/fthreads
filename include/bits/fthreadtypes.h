#ifndef FT_BITS_FTHREADTYPES_H
#define FT_BITS_FTHREADTYPES_H

/* fthread_t: opaque type */
struct _fthread;
typedef struct _fthread *fthread_t;

/* queue_t: needed for static initialization */
struct node;

typedef struct {
  struct node *__head;
  struct node *__tail;
} queue_t;
#define __QUEUE_INITIALIZER {0, 0}

/* fthread_mutex_t */
typedef struct {
  fthread_t    __holder;
  queue_t      __waitq;
  unsigned int __usecount;
} fthread_mutex_t;
#define FTHREAD_MUTEX_INITIALIZER {0, __QUEUE_INITIALIZER, 0}

/* fthread_cond_t */
typedef struct {
  fthread_mutex_t *__waitm;
  queue_t          __waitq;
  unsigned int     __usecount;
} fthread_cond_t;
#define FTHREAD_COND_INITIALIZER {0, __QUEUE_INITIALIZER, 0}

/* fthread_barrier_t */
typedef struct {
  queue_t      __waitq;
  unsigned int __waiting;
  unsigned int __max;
} fthread_barrier_t;
#define FTHREAD_BARRIER_INITIALIZER(MAX) {FTHREAD_COND_INITIALIZER, 0, MAX, 0}
#define FTHREAD_BARRIER_SERIAL_THREAD 0xf712ead5
  
#endif
