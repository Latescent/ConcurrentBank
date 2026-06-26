#include <pthread.h>
#include <stdlib.h>

#include "../include/bank.h"
#include "../include/logger.h"

static Account accounts[NUM_ACCOUNTS];

void init_bank() {
  for (int i = 0; i < NUM_ACCOUNTS; i++) {
    accounts[i].id = i;
    accounts[i].balance = INITIAL_BALANCE;

    pthread_mutex_init(&accounts[i].lock, NULL);
  }
}

int transfer_money(int from, int to, int amount) {
  int return_val;
  if (from == to)
    return -1;

  if (from < 0 || from >= NUM_ACCOUNTS)
    return -1;

  if (to < 0 || to >= NUM_ACCOUNTS)
    return -1;

  int first = (from < to) ? from : to;
  int second = (from < to) ? to : from;

  pthread_mutex_lock(&accounts[first].lock);
  pthread_mutex_lock(&accounts[second].lock);

  if (accounts[from].balance >= amount) {
    accounts[from].balance -= amount;
    accounts[to].balance += amount;

    return_val = 1;
  } else {
    return_val = 0;
  }

  pthread_mutex_unlock(&accounts[second].lock);
  pthread_mutex_unlock(&accounts[first].lock);

  return return_val;
}

int deposit_money(int account, int amount) {
  if (account < 0 || account >= NUM_ACCOUNTS)
    return 0;

  pthread_mutex_lock(&accounts[account].lock);

  accounts[account].balance += amount;

  pthread_mutex_unlock(&accounts[account].lock);

  return 1;
}

int withdraw_money(int account, int amount) {
  if (account < 0 || account >= NUM_ACCOUNTS)
    return 0;

  int success = 0;

  pthread_mutex_lock(&accounts[account].lock);

  if (accounts[account].balance >= amount) {
    accounts[account].balance -= amount;
    success = 1;
  }

  pthread_mutex_unlock(&accounts[account].lock);

  return success;
}

int get_balance(int account) {
  if (account < 0 || account >= NUM_ACCOUNTS)
    return -1;

  int balance;

  pthread_mutex_lock(&accounts[account].lock);

  balance = accounts[account].balance;

  pthread_mutex_unlock(&accounts[account].lock);

  return balance;
}

void print_bank() {
  log_printf("========== BANK ==========\n");

  for (int i = 0; i < NUM_ACCOUNTS; i++) {
    log_printf("Account %2d : %6d\n", i, get_balance(i));
  }

  log_printf("--------------------------\n");
  log_printf("Total = %d\n", total_balance());

  log_printf("==========================\n");
}

int total_balance() {
  int total = 0;

  for (int i = 0; i < NUM_ACCOUNTS; i++) {
    pthread_mutex_lock(&accounts[i].lock);
  }

  for (int i = NUM_ACCOUNTS - 1; i >= 0; i--) {
    total += accounts[i].balance;
    pthread_mutex_unlock(&accounts[i].lock);
  }

  return total;
}
