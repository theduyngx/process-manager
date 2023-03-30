# flags
CC = gcc
INC = -Iinclude/
CFLAGS = -Wall
CFLAGS += $(INC)

# paths
SRC_DIR = src
PRC_DIR = other
PROCESS = process.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

# executables
EXE = run
EXE_PRG = allocate
EXE_PRC = process

$(EXE): $(EXE_PRG) $(EXE_PRC)

$(EXE_PRG): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ -g

$(EXE_PRC): $(PRC_DIR)/$(PROCESS)
	$(CC) $(CFLAGS) $< -o $@ -g

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(EXE_PRG) $(EXE_PRC) $(SRC_DIR)/*.o