#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../include/common.h"

int main(void) {
  mkdir("logs", 0777);

  // Del FIFO
  unlink(FIFO_PATH);

  if (mkfifo(FIFO_PATH, 0666) == -1) {
    perror("mkfifo");
    return EXIT_FAILURE;
  }

  printf("FIFO created.\n");

  // Start Bank
  pid_t bank_pid = fork();

  if (bank_pid < 0) {
    perror("fork");
    unlink(FIFO_PATH);
    return EXIT_FAILURE;
  }

  if (bank_pid == 0) {
    execl("./bank_main", "bank_main", NULL);

    perror("execl(bank_main)");
    exit(EXIT_FAILURE);
  }

  printf("Bank started. PID = %d\n", bank_pid);

  // Start Customers
  pid_t customer_pids[MAX_CUSTOMERS];

  for (int i = 0; i < MAX_CUSTOMERS; i++) {
    pid_t pid = fork();

    if (pid < 0) {
      perror("fork");
      continue;
    }

    if (pid == 0) {
      char id[16];

      snprintf(id, sizeof(id), "%d", i);

      execl("./customer", "customer", id, NULL);

      perror("execl(customer)");
      exit(EXIT_FAILURE);
    }

    customer_pids[i] = pid;

    printf("Customer %d started (PID=%d)\n", i, pid);
  }

  // Wait for customers
  for (int i = 0; i < MAX_CUSTOMERS; i++) {
    waitpid(customer_pids[i], NULL, 0);

    printf("Customer %d finished.\n", i);
  }

  // Shutdown Bank
  printf("\n");
  printf("=====================================\n");
  printf("All customers have finished.\n");
  printf("Press ENTER to shutdown the bank...");
  fflush(stdout);

  getchar();

  printf("\nSending SIGINT to bank...\n");

  if (kill(bank_pid, SIGINT) == -1) {
    perror("kill");
  }

  waitpid(bank_pid, NULL, 0);

  printf("Bank stopped.\n");

  // Cleanup
  unlink(FIFO_PATH);

  printf("FIFO removed.\n");

  printf("Done.\n");

  return EXIT_SUCCESS;
}
