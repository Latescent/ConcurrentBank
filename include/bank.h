#ifndef BANK_H
#define BANK_H

#include "common.h"

typedef struct {
  int id;
  int balance;

  pthread_mutex_t lock;

} Account;

void init_bank();

int transfer_money(int from, int to, int amount);

int deposit_money(int account, int amount);

int withdraw_money(int account, int amount);

int get_balance(int account);

void print_bank();

int total_balance();

#endif
