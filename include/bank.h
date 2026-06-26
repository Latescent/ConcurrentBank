#ifndef BANK_H
#define BANK_H

#include <pthread.h>

typedef struct {
  int id;
  int balance;
  pthread_mutex_t lock;
} Account;

#endif
