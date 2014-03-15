#include "ft_queue.h"

#include <stdlib.h>

void queue_init(queue_t *q) {
  q->__head = 0;
  q->__tail = 0;
}

void queue_destroy(queue_t *q) {
  struct node *c, *next;
  for (c = q->__head; c != 0; c = next) {
    next = c->next;
    free(c);
  }
}

void queue_enqueue(queue_t *q, void *data) {
  struct node *n = (struct node *)malloc(sizeof(struct node));
  n->data = data;
  n->next = 0;
  if (!q->__head) {
    q->__head = q->__tail = n;
  } else {
    q->__tail->next = n;
    q->__tail = n;
  }
}

void *queue_dequeue(queue_t *q) {
  if (!q->__head) {
    return 0;
  }
  struct node *n = q->__head;
  q->__head = q->__head->next;
  void *dcopy = n->data;
  free(n);
  return dcopy;
}

int queue_empty(queue_t *q) {
  return !q->__head;
}
