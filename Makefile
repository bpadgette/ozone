##############################################################################
# Target
DIST        := ozone
DIST_DEBUG  := $(DIST).debug

##############################################################################
# Toolchain
#
ARCHIVER          := ar rcs
CC                := gcc
CLEAN             := rm -rf
COPY              := cp
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
$(shell $(MKDIR) $(BUILD))
$(shell $(MKDIR) $(LIB))

##############################################################################
# Build & Run
#
CFLAGS        := -Wall -Werror -Wextra -pedantic -O3 -I$(INCLUDE)
CLIBS         := -lm
OBJECTS       := $(patsubst $(SOURCE)%.c, $(BUILD)%.o, $(filter-out $(SOURCE)$(DIST).c, $(wildcard *, $(SOURCE)*.c)))
DEBUG_OBJECTS := $(patsubst $(SOURCE)%.c, $(BUILD)%.debug.o, $(filter-out $(SOURCE)$(DIST).c, $(wildcard *, $(SOURCE)*.c)))

$(BUILD)%.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -c $< $(CLIBS) -o $@

$(BUILD)%.debug.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -DOZONE_LOG_DEBUG -g -c $< $(CLIBS) -o $@

$(BUILD)$(DIST).h:
	$(COPY) $(INCLUDE)$(DIST).h $(BUILD)$(DIST).h

$(BUILD)$(DIST): $(OBJECTS) $(BUILD)$(DIST).h
	$(ARCHIVER) $(BUILD)$(DIST).a $(OBJECTS)

$(BUILD)$(DIST_DEBUG): $(DEBUG_OBJECTS) $(BUILD)$(DIST).h
	$(ARCHIVER) $(BUILD)$(DIST_DEBUG).a $(DEBUG_OBJECTS)

build: $(BUILD)$(DIST)
build-debug: $(BUILD)$(DIST_DEBUG)

##############################################################################
# Testing Libraries
#  - Unity-2.6.0
#
TEST_LIB_INCLUDES := $(LIB)Unity-2.6.0/src
TEST_LIB_SOURCES  := $(LIB)Unity-2.6.0/src
$(TEST_LIB_SOURCES):
	$(DOWNLOAD_TARBALL) https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.0.tar.gz | $(UNTAR_INTO) $(LIB)

##############################################################################
# Testing
#
TEST      := $(ROOT)test/
TESTS     := $(patsubst $(TEST)%.c, $(BUILD)%, $(wildcard *, $(TEST)*.test.c))

TEST_LIB  := $(LIB)unity.o
$(TEST_LIB): $(TEST_LIB_SOURCES)
	$(CC) $(CFLAGS) -c $(LIB)Unity-2.6.0/src/unity.c $(CLIBS) -o $@

$(BUILD)%.test: $(TEST)%.test.c $(DEBUG_OBJECTS) $(TEST_LIB)
	$(CC) $(CFLAGS) $(foreach inc, $(TEST_LIB_INCLUDES), -I$(inc)) -g $^ $(CLIBS) -o $@
	$@

test: $(TESTS)

##############################################################################
# Examples
#
EXAMPLES    	  := $(ROOT)examples/
BUILD_EXAMPLES    := $(BUILD)examples/

$(BUILD_EXAMPLES)%: $(BUILD)$(DIST).a
	$(shell $(MKDIR) $(BUILD_EXAMPLES))
	$(CC) $(CFLAGS) $(EXAMPLES)$*.c $< $(CLIBS) -o $@

$(BUILD_EXAMPLES)%.debug: $(BUILD)$(DIST_DEBUG).a
	$(shell $(MKDIR) $(BUILD_EXAMPLES))
	$(CC) $(CFLAGS) -DOZONE_LOG_DEBUG -g $(EXAMPLES)$*.c $< $(CLIBS) -o $@

start: $(BUILD_EXAMPLES)start
	$<

debug: $(BUILD_EXAMPLES)start.debug
	$(DEBUGGER) $<

memcheck: $(BUILD_EXAMPLES)start.debug
	$(MEMCHECK) $<

##############################################################################
# Helpers
#
clean:
	$(CLEAN) $(BUILD)

all: build build-debug $(TESTS)

.PHONY: build build-debug test debug memcheck clean start all
