CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -g -Iinclude
LDFLAGS = -pthread

SRC_DIR = src
BUILD_DIR = build

COMMON_OBJS = \
	$(BUILD_DIR)/bank.o \
	$(BUILD_DIR)/queue.o

all: directories \
	build/bank_main \
	build/customer \
	build/entry_point

#################################################
# Create build directory if needed
#################################################

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p logs

#################################################
# Executables
#################################################

build/bank_main: \
	$(BUILD_DIR)/bank_main.o \
	$(COMMON_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

build/customer: \
	$(BUILD_DIR)/customer.o
	$(CC) $^ -o $@ $(LDFLAGS)

build/entry_point: \
	$(BUILD_DIR)/entry_point.o
	$(CC) $^ -o $@ $(LDFLAGS)

#################################################
# Object Files
#################################################

$(BUILD_DIR)/bank_main.o: $(SRC_DIR)/bank_main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/customer.o: $(SRC_DIR)/customer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/entry_point.o: $(SRC_DIR)/entry_point.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/bank.o: $(SRC_DIR)/bank.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/queue.o: $(SRC_DIR)/queue.c
	$(CC) $(CFLAGS) -c $< -o $@

#################################################
# Utility Targets
#################################################

run: clearn all
	./build/entry_point

clean:
	rm -rf build
	rm -rf logs

.PHONY: all clean run directories
