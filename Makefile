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

# paths
SRC_DIR = src
T1_DIR  = cases/task1
T2_DIR  = cases/task2
T3_DIR  = cases/task3
T4_DIR  = cases/task4
PRC_DIR = process
PROCESS = process.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

# executables
EXE = run
EXE_PRG = allocate
EXE_PRC = process

# make executable
$(EXE): $(EXE_PRG) $(EXE_PRC)

$(EXE_PRG): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ -g

$(EXE_PRC): $(PRC_DIR)/$(PROCESS)
	$(CC) $(CFLAGS) $< -o $@ -g

# object files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

# other commands
clean:
	rm -f $(EXE_PRG) $(EXE_PRC) $(SRC_DIR)/*.o $(PRC_DIR)/*.out

two-processes-1:
	./allocate -f $(T2_DIR)/two-processes.txt $(RR) $(INF) -q 1 > $(PRC_DIR)/$@.out
	diff $(PRC_DIR)/$@.out $(T2_DIR)/$@.out || true

two-processes-3:
	./allocate -f $(T2_DIR)/two-processes.txt $(RR) $(INF) -q 3 > $(PRC_DIR)/$@.out
	diff $(PRC_DIR)/$@.out $(T2_DIR)/$@.out || true

simple-rr:
	./allocate -f $(T2_DIR)/simple.txt $(RR) $(INF) -q 3 > $(PRC_DIR)/$@.out
	diff $(PRC_DIR)/$@.out $(T2_DIR)/$@.out || true

non-fit-rr:
	./allocate -f $(T3_DIR)/non-fit.txt $(RR) $(BF) -q 3 > $(PRC_DIR)/$@.out
	diff $(PRC_DIR)/$@.out $(T3_DIR)/$@.out || true

non-fit-sjf:
	./allocate -f $(T3_DIR)/non-fit.txt $(SJF) $(BF) -q 3 > $(PRC_DIR)/$@.out
	diff $(PRC_DIR)/$@.out $(T3_DIR)/$@.out || true

simple-bestfit:
	./allocate -f $(T3_DIR)/simple.txt $(RR) $(BF) -q 3 > $(PRC_DIR)/$@.out
	diff $(PRC_DIR)/$@.out $(T3_DIR)/$@.out || true
