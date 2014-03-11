#include "ft_queue.h"

#include <stdlib.h>

struct node {
  void        *data;
  struct node *next;
};

void queue_init(struct queue *q) {
  q->head = 0;
  q->tail = 0;
}

void queue_destroy(struct queue *q) {
  struct node *c, *next;
  for (c = q->head; c != 0; c = next) {
    next = c->next;
    free(c);
  }
  free(q);
}

void queue_enqueue(struct queue *q, void *data) {
  struct node *n = (struct node *)malloc(sizeof(struct node));
  n->data = data;
  n->next = 0;
  if (!q->head) {
    q->head = q->tail = n;
  } else {
    q->tail->next = n;
    q->tail = n;
  }
}

void *queue_dequeue(struct queue *q) {
  if (!q->head) {
    return 0;
  }
  struct node *n = q->head;
  q->head = q->head->next;
  void *dcopy = n->data;
  free(n);
  return dcopy;
}

int queue_empty(struct queue *q) {
  return !q->head;
}
