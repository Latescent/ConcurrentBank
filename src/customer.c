#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/logger.h"

static int rand_account(void) { return rand() % NUM_ACCOUNTS; }

static int rand_amount(void) { return rand() % 200 + 1; }

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <customer_id>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int customer_id = atoi(argv[1]);

  srand(time(NULL) ^ getpid());

  int fd = open(FIFO_PATH, O_WRONLY);

  if (fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  log_printf("Customer %d started.\n", customer_id);

  for (int i = 0; i < REQUESTS_PER_CUSTOMER; i++) {
    BankRequest req;

    req.customer_id = customer_id;
    req.amount = rand_amount();

    int r = rand() % 100;
    if (r < 50) {

      req.type = TRANSFER;

      req.account1 = rand_account();

      do {
        req.account2 = rand_account();
      } while (req.account1 == req.account2);

      log_printf("[Customer %d] TRANSFER %d -> %d (%d)\n", customer_id,
                 req.account1, req.account2, req.amount);

    } else if (r < 70) {

      req.type = DEPOSIT;

      req.account1 = rand_account();
      req.account2 = -1;
      // req.amount = 0;

      log_printf("[Customer %d] DEPOSIT %d (%d)\n", customer_id, req.account1,
                 req.amount);

    } else if (r < 90) {

      req.type = WITHDRAW;

      req.account1 = rand_account();
      req.account2 = -1;
      // req.amount = 0;

      log_printf("[Customer %d] WITHDRAW %d (%d)\n", customer_id, req.account1,
                 req.amount);

    } else {

      req.type = BALANCE;

      req.account1 = rand_account();
      req.account2 = -1;
      req.amount = 0;

      log_printf("[Customer %d] BALANCE %d\n", customer_id, req.account1);
    }

    if (write(fd, &req, sizeof(req)) != sizeof(req)) {
      perror("write");
      break;
    }

    usleep((rand() % 500 + 250) * 1000);
  }

  close(fd);

  log_printf("Customer %d exiting.\n", customer_id);

  return EXIT_SUCCESS;
}
