#include "../include/logger.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/file.h>
#include <time.h>
#include <unistd.h>

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static void print_prefix(FILE *stream) {
  time_t now = time(NULL);

  struct tm tm_now;
  localtime_r(&now, &tm_now);

  char buffer[32];

  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_now);

  fprintf(stream, "[%s][PID:%d][T%lu] ", buffer, getpid(),
          (unsigned long)pthread_self());
}

void logger_init(const char *filename) {
  log_file = fopen(filename, "a");

  if (log_file == NULL) {
    perror("logger fopen");
  }
}

void logger_close(void) {
  pthread_mutex_lock(&log_mutex);

  if (log_file) {
    fclose(log_file);
    log_file = NULL;
  }

  pthread_mutex_unlock(&log_mutex);
}

void log_printf(const char *fmt, ...) {
  pthread_mutex_lock(&log_mutex);

  /* ---------- Terminal ---------- */

  print_prefix(stdout);

  va_list args;

  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  fflush(stdout);

  /* ---------- Log file ---------- */

  if (log_file) {
    flock(fileno(log_file), LOCK_EX);

    print_prefix(log_file);

    va_start(args, fmt);
    vfprintf(log_file, fmt, args);
    va_end(args);

    fflush(log_file);

    flock(fileno(log_file), LOCK_UN);
  }

  pthread_mutex_unlock(&log_mutex);
}
