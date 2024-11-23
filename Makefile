##############################################################################
# Target
EXE        := serve
EXE_DEBUG  := $(EXE).debug

##############################################################################
# Toolchain
#
CC                := gcc
CLEAN             := rm -rf
DEBUGGER          := gdb
DOWNLOAD_TARBALL  := curl -s -L
MEMCHECK          := valgrind --leak-check=yes
MKDIR             := mkdir -p
UNTAR_INTO        := tar xz -C

##############################################################################
# Paths
#
ROOT        := $(CURDIR)/
SOURCE      := $(ROOT)src/
INCLUDE     := $(ROOT)include/
BUILD       := $(ROOT)build/
LIB         := $(ROOT)lib/
LIB_BUILD   := $(BUILD)lib/
$(shell $(MKDIR) $(BUILD))
$(shell $(MKDIR) $(LIB))
$(shell $(MKDIR) $(LIB_BUILD))

##############################################################################
# Libraries
#  - Unity-2.6.0  Testing library
#
LIB_INCLUDES := $(LIB)Unity-2.6.0/src
LIB_SOURCES  := $(LIB)Unity-2.6.0/src
$(LIB_SOURCES):
	$(DOWNLOAD_TARBALL) https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.0.tar.gz | $(UNTAR_INTO) $(LIB)

##############################################################################
# Build & Run
#
CFLAGS        := -Wall -O3$(foreach inc, $(LIB_INCLUDES), -I$(inc)) -I$(INCLUDE)
OBJECTS       := $(patsubst $(SOURCE)%.c, $(BUILD)%.o, $(filter-out $(SOURCE)$(EXE).c, $(wildcard *, $(SOURCE)*.c)))
DEBUG_OBJECTS := $(patsubst $(SOURCE)%.c, $(BUILD)%.debug.o, $(filter-out $(SOURCE)$(EXE).c, $(wildcard *, $(SOURCE)*.c)))

$(BUILD)%.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BUILD)%.debug.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -DOZ_LOG_TRACE -g -c $< -o $@ 

$(EXE): $(OBJECTS)
	$(CC) $(CFLAGS) -static $(SOURCE)$(EXE).c $^ -o $(BUILD)$(EXE)

$(EXE_DEBUG): $(DEBUG_OBJECTS)
	$(CC) $(CFLAGS) -DOZ_LOG_TRACE -g  $(SOURCE)$(EXE).c $^ -o $(BUILD)$(EXE_DEBUG)

build: $(EXE)
build-debug: $(EXE_DEBUG)

##############################################################################
# Testing
#
TEST      := $(ROOT)test/
TESTS     := $(patsubst $(TEST)%.c, $(BUILD)%, $(wildcard *, $(TEST)*.test.c))

TEST_LIB  := $(LIB_BUILD)unity.o
$(TEST_LIB): $(LIB_SOURCES)
	$(CC) $(CFLAGS) -c $(LIB)Unity-2.6.0/src/unity.c -o $@

$(BUILD)%.test: $(TEST)%.test.c $(DEBUG_OBJECTS) $(TEST_LIB)
	$(CC) $(CFLAGS) -g $^ -o $@
	$@

test: $(TESTS)

##############################################################################
# Helpers
#
clean:
	$(CLEAN) $(BUILD)

debug: build-debug
	$(DEBUGGER) $(BUILD)$(EXE_DEBUG)

memcheck: build-debug
	$(MEMCHECK) $(BUILD)$(EXE_DEBUG)

start: build
	$(BUILD)$(EXE)

all: build build-debug $(TESTS)
	cd $(BUILD) && ls -lh

.PHONY: build build-debug start test debug memcheck clean all
