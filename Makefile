ROOT	:= $(CURDIR)
SOURCE	:= $(ROOT)/src/
INCLUDE	:= $(ROOT)/include/
BUILD	:= $(ROOT)/build/
$(shell mkdir -p $(BUILD))

EXE				:= webserver
EXE_DEBUG		:= $(EXE).debug

CC				:= gcc
CFLAGS			:= -Wall -O3 -I$(INCLUDE)
OBJECTS			:= $(patsubst $(INCLUDE)%.h, $(BUILD)%.o, $(wildcard *, $(INCLUDE)*.h))

$(BUILD)%.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) -static $(SOURCE)$(EXE).c $^ -o $(BUILD)$(EXE)

$(EXE_DEBUG): $(OBJECTS)
	$(CC) $(CFLAGS) -g $(SOURCE)$(EXE).c $^ -o $(BUILD)$(EXE_DEBUG)

memcheck: $(EXE_DEBUG)
	valgrind --leak-check=yes $(BUILD)$(EXE_DEBUG)

clean:
	rm -rf $(BUILD)

run: $(EXE)
	$(BUILD)$(EXE)

all: $(EXE) $(EXE_DEBUG)
	cd $(BUILD) && ls -lh

.PHONY: memcheck clean run all
