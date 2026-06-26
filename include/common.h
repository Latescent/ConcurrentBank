#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>

#define FIFO_PATH "/tmp/bank_requests"

#define NUM_ACCOUNTS 10
#define INITIAL_BALANCE 1000

#define NUM_WORKERS 4
#define MAX_QUEUE_SIZE 100

#define MAX_CUSTOMERS 5
#define REQUESTS_PER_CUSTOMER 20

typedef enum { TRANSFER, DEPOSIT, WITHDRAW, BALANCE, SHUTDOWN } RequestType;

typedef struct {
  RequestType type;

  int customer_id;

  int from_account;
  int to_account;

  int amount;

} BankRequest;

#endif
