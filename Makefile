CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -g -Iinclude
LDFLAGS = -pthread

SRC_DIR = src
BUILD_DIR = build

#################################################
# Object Groups
#################################################

BANK_OBJS = \
	$(BUILD_DIR)/bank_main.o \
	$(BUILD_DIR)/bank.o \
	$(BUILD_DIR)/queue.o \
	$(BUILD_DIR)/logger.o

ENTRY_OBJS = \
	$(BUILD_DIR)/entry_point.o \
	$(BUILD_DIR)/logger.o

CUSTOMER_OBJS = \
	$(BUILD_DIR)/customer.o \
	$(BUILD_DIR)/logger.o

#################################################
# Default Target
#################################################

all: directories \
	$(BUILD_DIR)/bank_main \
	$(BUILD_DIR)/customer \
	$(BUILD_DIR)/entry_point

#################################################
# Directories
#################################################

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p logs

#################################################
# Executables
#################################################

$(BUILD_DIR)/bank_main: $(BANK_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/customer: $(CUSTOMER_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/entry_point: $(ENTRY_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

#################################################
# Object Files
#################################################

$(BUILD_DIR)/bank_main.o: $(SRC_DIR)/bank_main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/bank.o: $(SRC_DIR)/bank.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/queue.o: $(SRC_DIR)/queue.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/customer.o: $(SRC_DIR)/customer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/entry_point.o: $(SRC_DIR)/entry_point.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/logger.o: $(SRC_DIR)/logger.c
	$(CC) $(CFLAGS) -c $< -o $@

#################################################
# Convenience Targets
#################################################

run: clean all
	./$(BUILD_DIR)/entry_point

clean:
	rm -rf $(BUILD_DIR)
	rm -rf logs

#################################################
# Debug Build (AddressSanitizer + UBSan)
#################################################

debug: CFLAGS += -fsanitize=address -fsanitize=undefined
debug: LDFLAGS += -fsanitize=address -fsanitize=undefined
debug: clean all

.PHONY: all directories run clean debug
