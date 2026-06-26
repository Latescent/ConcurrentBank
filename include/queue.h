#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

typedef struct {
  TransferRequest requests[MAX_QUEUE_SIZE];

  int front;
  int rear;
  int count;

  pthread_mutex_t mutex;

  pthread_cond_t not_empty;
  pthread_cond_t not_full;

} RequestQueue;

void queue_init(RequestQueue *q);

void enqueue(RequestQueue *q, TransferRequest request);

TransferRequest dequeue(RequestQueue *q);

#endif
