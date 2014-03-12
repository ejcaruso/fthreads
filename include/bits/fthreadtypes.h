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
  fthread_t __holder;
  queue_t   __waitq;
  int       __usecount;
} fthread_mutex_t;
#define FTHREAD_MUTEX_INITIALIZER {0, __QUEUE_INITIALIZER, 0}

/* fthread_cond_t */
typedef struct {
  fthread_mutex_t *__waitm;
  queue_t          __waitq;
  int              __usecount;
} fthread_cond_t;
#define FTHREAD_COND_INITIALIZER {0, __QUEUE_INITIALIZER, 0}
  
#endif
