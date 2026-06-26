#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/bank.h"

static Account accounts[NUM_ACCOUNTS];

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static void log_transaction(const char *message) {
  pthread_mutex_lock(&log_mutex);

  if (log_file != NULL) {
    fprintf(log_file, "%s\n", message);
    fflush(log_file);
  }

  pthread_mutex_unlock(&log_mutex);
}

void init_bank() {
  for (int i = 0; i < NUM_ACCOUNTS; i++) {
    accounts[i].id = i;
    accounts[i].balance = INITIAL_BALANCE;

    pthread_mutex_init(&accounts[i].lock, NULL);
  }

  log_file = fopen("logs/transactions.log", "w");

  if (log_file == NULL) {
    perror("Cannot create log file");
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

  char buffer[256];

  if (accounts[from].balance >= amount) {
    accounts[from].balance -= amount;
    accounts[to].balance += amount;

    return_val = 1;
  } else {
    return_val = 0;
  }

  pthread_mutex_unlock(&accounts[second].lock);
  pthread_mutex_unlock(&accounts[first].lock);

  log_transaction(buffer);

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
  printf("\n========== BANK ==========\n");

  for (int i = 0; i < NUM_ACCOUNTS; i++) {
    printf("Account %2d : %6d\n", i, get_balance(i));
  }

  printf("--------------------------\n");
  printf("Total = %d\n", total_balance());

  printf("==========================\n");
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
