#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/bank.h"
#include "../include/common.h"
#include "../include/queue.h"

volatile sig_atomic_t running = 1;

void sigint_handler(int sig) { running = 0; }

RequestQueue queue;

void *reader_thread(void *arg) {
  int fd = open(FIFO_PATH, O_RDWR);

  if (fd < 0) {
    perror("open");
    pthread_exit(NULL);
  }

  printf("Reader thread started.\n");

  while (running) {
    BankRequest req;

    ssize_t bytes = read(fd, &req, sizeof(req));

    if (bytes == 0) {
      printf("All customers disconnected.\n");

      break;
    }

    if (bytes < 0) {
      if (errno == EINTR && !running)
        break;

      perror("read");
      continue;
    }

    if (bytes != sizeof(req))
      continue;

    enqueue(&queue, req);
  }

  close(fd);

  BankRequest stop;

  stop.type = SHUTDOWN;

  for (int i = 0; i < NUM_WORKERS; i++)
    enqueue(&queue, stop);

  pthread_exit(NULL);
}

void *worker_thread(void *arg) {
  int id = *(int *)arg;
  int t_success;

  printf("Worker %d started.\n", id);

  while (1) {
    BankRequest req = dequeue(&queue);

    switch (req.type) {
    case TRANSFER:

      printf("[Worker %d] Transfer\n", id);

      t_success = transfer_money(req.account1, req.account2, req.amount);
      printf(
          "WORKER %d | CUSTOMER %d | TRANSFER | %d -> %d | Amount = %d | %s\n",
          id, req.customer_id, req.account1, req.account2, req.amount,
          t_success ? "SUCCESS" : "FAILED");

      break;

    case DEPOSIT:

      t_success = deposit_money(req.account2, req.amount);
      printf(
          "WORKER %d | CUSTOMER %d | DEPOSIT | Account %d | Amount = %d | %s\n",
          id, req.customer_id, req.account2, req.amount,
          t_success ? "SUCCESS" : "FAILED");
      break;

    case WITHDRAW:

      t_success = withdraw_money(req.account1, req.amount);
      printf("WORKER %d | CUSTOMER %d | WITHDRAW | Account %d | Amount = %d | "
             "%s\n",
             id, req.customer_id, req.account2, req.amount,
             t_success ? "SUCCESS" : "FAILED");
      break;

    case BALANCE:

      printf("WORKER %d | CUSTOMER %d | BALANCE | Account %d | Amount = %d\n",
             id, req.customer_id, req.account1, get_balance(req.account1));
      break;

    case SHUTDOWN:

      printf("Worker %d exiting.\n", id);

      pthread_exit(NULL);
    }
  }
}

int main() {
  printf("========== BANK ==========\n");

  // SIGINT
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));

  sa.sa_handler = sigint_handler;
  sigemptyset(&sa.sa_mask);

  sa.sa_flags = 0;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("sigaction SIGTERM");
    exit(EXIT_FAILURE);
  }

  // ------

  init_bank();

  queue_init(&queue);

  pthread_t reader;

  pthread_create(&reader, NULL, reader_thread, NULL);

  pthread_t workers[NUM_WORKERS];

  int ids[NUM_WORKERS];

  for (int i = 0; i < NUM_WORKERS; i++) {
    ids[i] = i;

    pthread_create(&workers[i], NULL, worker_thread, &ids[i]);
  }

  pthread_join(reader, NULL);

  for (int i = 0; i < NUM_WORKERS; i++)
    pthread_join(workers[i], NULL);

  printf("\n");

  print_bank();

  printf("Final total = %d\n", total_balance());

  printf("Bank shutting down.\n");

  return 0;
}
