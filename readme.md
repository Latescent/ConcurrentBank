# Concurrent Bank

A simple concurrent bank simulation written in C for an Operating Systems laboratory course.

## Features

- Multiple customer processes (`fork` + `exec`)
- Bank process with a pool of worker threads (`pthreads`)
- Inter-process communication using a POSIX named pipe (FIFO)
- Producer-consumer request queue
- Thread-safe bank operations using mutexes
- Deadlock-free money transfers
- Graceful shutdown using `SIGINT`
- Timestamped logging with process and thread identifiers

## Project Structure

```
.
├── include/
│   ├── bank.h
│   ├── common.h
│   ├── logger.h
│   └── queue.h
├── src/
│   ├── bank.c
│   ├── bank_main.c
│   ├── customer.c
│   ├── entry_point.c
│   ├── logger.c
│   └── queue.c
├── build/
├── logs/
└── Makefile
```

## Building

```bash
make
```

## Running

```bash
make run
```

The program will:

1. Create the FIFO.
2. Launch the bank process.
3. Launch multiple customer processes.
4. Process requests concurrently.
5. Wait for all customers to finish.
6. Press **Enter** to shut down the bank gracefully.

## Cleaning

```bash
make clean
```
