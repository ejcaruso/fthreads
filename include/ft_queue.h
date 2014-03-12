#ifndef FT_QUEUE_H
#define FT_QUEUE_H

#include "bits/fthreadtypes.h"

struct node {
  void        *data;
  struct node *next;
};

void  queue_init(queue_t *q);
void  queue_destroy(queue_t *q);

void  queue_enqueue(queue_t *q, void *data);
void *queue_dequeue(queue_t *q);
int   queue_empty(queue_t *q);

#endif
