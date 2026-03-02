CC = gcc
CFLAGS = -Wall -g -Iinclude
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Target executables
TARGETS = $(BIN_DIR)/server $(BIN_DIR)/client

all: $(TARGETS)

$(BIN_DIR)/server: $(SRC_DIR)/server_main.c $(SRC_DIR)/network_utils.c $(SRC_DIR)/proc_utils.c
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/client: $(SRC_DIR)/client_main.c $(SRC_DIR)/network_utils.c $(SRC_DIR)/history.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BIN_DIR)/*