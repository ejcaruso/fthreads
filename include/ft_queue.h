#ifndef FT_QUEUE_H
#define FT_QUEUE_H

struct node;

typedef struct queue {
  struct node *head;
  struct node *tail;
} queue_t;

#define STATIC_QUEUE_INITIALIZER {0, 0}

void  queue_init(queue_t *q);
void  queue_destroy(queue_t *q);

void  queue_enqueue(queue_t *q, void *data);
void *queue_dequeue(queue_t *q);
int   queue_empty(queue_t *q);

#endif
