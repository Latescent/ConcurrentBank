#include "../include/queue.h"

void queue_init(RequestQueue *q) {
  q->front = 0;
  q->rear = 0;
  q->count = 0;

  pthread_mutex_init(&q->mutex, NULL);

  pthread_cond_init(&q->not_empty, NULL);
  pthread_cond_init(&q->not_full, NULL);
}

void enqueue(RequestQueue *q, BankRequest request) {
  pthread_mutex_lock(&q->mutex);

  while (q->count == MAX_QUEUE_SIZE) {
    pthread_cond_wait(&q->not_full, &q->mutex);
  }

  q->requests[q->rear] = request;

  q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;

  q->count++;

  pthread_cond_signal(&q->not_empty);

  pthread_mutex_unlock(&q->mutex);
}

BankRequest dequeue(RequestQueue *q) {
  pthread_mutex_lock(&q->mutex);

  while (q->count == 0) {
    pthread_cond_wait(&q->not_empty, &q->mutex);
  }

  BankRequest request = q->requests[q->front];

  q->front = (q->front + 1) % MAX_QUEUE_SIZE;

  q->count--;

  pthread_cond_signal(&q->not_full);

  pthread_mutex_unlock(&q->mutex);

  return request;
}
