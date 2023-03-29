# flags
CC = gcc
INC = -Iinclude/
CFLAGS = -Wall
CFLAGS += $(INC)

# paths
BIN = bin
OBJ_DIR = $(BIN)/obj
SRC_DIR = src
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
EXE = main

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ -g

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(EXE) $(OBJ_DIR)/*.o $(SRC_DIR)/*.o