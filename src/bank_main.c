#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/bank.h"
#include "../include/common.h"
#include "../include/logger.h"
#include "../include/queue.h"

volatile sig_atomic_t running = 1;

RequestQueue queue;

void sigint_handler(int sig) {
  (void)sig;
  running = 0;
}

void *reader_thread(void *arg) {
  (void)arg;

  int fd = open(FIFO_PATH, O_RDWR);

  if (fd == -1) {
    perror("open");
    return NULL;
  }

  log_printf("Reader thread started.\n");

  struct pollfd pfd;
  pfd.fd = fd;
  pfd.events = POLLIN;

  while (running) {
    int ret = poll(&pfd, 1, 100);

    if (ret == 0)
      continue;

    if (ret < 0) {
      if (errno == EINTR)
        continue;

      perror("poll");
      break;
    }

    if (pfd.revents & POLLIN) {
      BankRequest req;

      ssize_t bytes = read(fd, &req, sizeof(req));

      if (bytes == sizeof(req)) {
        enqueue(&queue, req);
      } else if (bytes < 0 && errno != EINTR) {
        perror("read");
      }
    }
  }

  close(fd);

  log_printf("Reader thread exiting...\n");

  BankRequest stop;
  stop.type = SHUTDOWN;

  for (int i = 0; i < NUM_WORKERS; i++) {
    enqueue(&queue, stop);
  }

  return NULL;
}

void *worker_thread(void *arg) {
  int id = *(int *)arg;

  log_printf("Worker %d started.\n", id);

  while (1) {
    BankRequest req = dequeue(&queue);

    int success = 0;

    switch (req.type) {
    case TRANSFER:

      success = transfer_money(req.account1, req.account2, req.amount);

      log_printf(
          "WORKER %d | CUSTOMER %d | TRANSFER | %d -> %d | Amount = %d | %s\n",
          id, req.customer_id, req.account1, req.account2, req.amount,
          success ? "SUCCESS" : "FAILED");

      break;

    case DEPOSIT:

      success = deposit_money(req.account1, req.amount);

      log_printf(
          "WORKER %d | CUSTOMER %d | DEPOSIT | Account %d | Amount = %d | %s\n",
          id, req.customer_id, req.account1, req.amount,
          success ? "SUCCESS" : "FAILED");

      break;

    case WITHDRAW:

      success = withdraw_money(req.account1, req.amount);

      log_printf(
          "WORKER %d | CUSTOMER %d | WITHDRAW | Account %d | Amount = %d | "
          "%s\n",
          id, req.customer_id, req.account1, req.amount,
          success ? "SUCCESS" : "FAILED");

      break;

    case BALANCE:

      log_printf(
          "WORKER %d | CUSTOMER %d | BALANCE | Account %d | Balance = %d\n", id,
          req.customer_id, req.account1, get_balance(req.account1));

      break;

    case SHUTDOWN:

      log_printf("Worker %d exiting.\n", id);
      return NULL;
    }
  }
}

int main(void) {
  logger_init("logs/system.log");

  log_printf("========== BANK ==========\n");

  init_bank();

  queue_init(&queue);

  struct sigaction sa;

  sa.sa_handler = sigint_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  pthread_t reader;
  pthread_t workers[NUM_WORKERS];

  int ids[NUM_WORKERS];

  if (pthread_create(&reader, NULL, reader_thread, NULL)) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < NUM_WORKERS; i++) {
    ids[i] = i;

    if (pthread_create(&workers[i], NULL, worker_thread, &ids[i])) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
  }

  pthread_join(reader, NULL);

  for (int i = 0; i < NUM_WORKERS; i++) {
    pthread_join(workers[i], NULL);
  }

  log_printf("========== FINAL STATE ==========\n");

  print_bank();

  log_printf("Bank shutdown complete.\n");

  return EXIT_SUCCESS;
}
