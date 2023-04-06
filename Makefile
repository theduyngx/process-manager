# compiler flags
CC 		= gcc
INC 	= -Iinclude/
CFLAGS  = -Wall
CFLAGS += $(INC)

# program tags
RR		= -s RR
SJF		= -s SJF
BF 		= -m best-fit
INF		= -m infinite
Q1		= -q 1
Q2		= -q 2
Q3		= -q 3

# paths
SRC_DIR = src
T1_DIR  = cases/task1
T2_DIR  = cases/task2
T3_DIR  = cases/task3
T4_DIR  = cases/task4
OUT_DIR = out
PRC_DIR = process-real
PROCESS = process.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

# executables
EXE     = allocate
EXE_PRC = process

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ -g

$(EXE_PRC): $(PRC_DIR)/$(PROCESS)
	$(CC) $(CFLAGS) $< -o $@ -g


# object files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

# format
format:
	clang-format -style=file -i *.c


# other commands
clean:
	rm -f $(EXE) $(EXE_PRC) $(SRC_DIR)/*.o $(OUT_DIR)/*.out

two-processes-1:
	./$(EXE) -f $(T2_DIR)/two-processes.txt $(RR) $(INF) $(Q1) > $(OUT_DIR)/$@.out
	diff $(OUT_DIR)/$@.out $(T2_DIR)/$@.out || true

two-processes-3:
	./$(EXE) -f $(T2_DIR)/two-processes.txt $(RR) $(INF) $(Q3) > $(OUT_DIR)/$@.out
	diff $(OUT_DIR)/$@.out $(T2_DIR)/$@.out || true

simple-rr:
	./$(EXE) -f $(T2_DIR)/simple.txt $(RR) $(INF) $(Q3) > $(OUT_DIR)/$@.out
	diff $(OUT_DIR)/$@.out $(T2_DIR)/$@.out || true

non-fit-rr:
	./$(EXE) -f $(T3_DIR)/non-fit.txt $(RR) $(BF) $(Q3) > $(OUT_DIR)/$@.out
	diff $(OUT_DIR)/$@.out $(T3_DIR)/$@.out || true

non-fit-sjf:
	./$(EXE) -f $(T3_DIR)/non-fit.txt $(SJF) $(BF) $(Q3) > $(OUT_DIR)/$@.out
	diff $(OUT_DIR)/$@.out $(T3_DIR)/$@.out || true

simple-bestfit:
	./$(EXE) -f $(T3_DIR)/simple.txt $(RR) $(BF) $(Q3) > $(OUT_DIR)/$@.out
	diff $(OUT_DIR)/$@.out $(T3_DIR)/$@.out || true
