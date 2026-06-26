#ifndef COMMON_H
#define COMMON_H

#define FIFO_PATH "/tmp/bank_requests"

#define NUM_ACCOUNTS 10
#define INITIAL_BALANCE 1000

typedef struct {
  int customer_id;
  int from_account;
  int to_account;
  int amount;
} TransferRequest;

#endif
