# Makefile
CC = gcc
CFLAGS = -Wall
SRC_DIR = src
BIN_DIR = .

EXE = $(BIN_DIR)/snake
SRC = $(wildcard $(SRC_DIR)/*.c)

all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@
