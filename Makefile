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
BUILD       := $(ROOT)build/
INCLUDE     := $(ROOT)include/
LIB         := $(ROOT)lib/
SOURCE      := $(ROOT)src/
TEST        := $(ROOT)test/
$(shell $(MKDIR) $(BUILD))
$(shell $(MKDIR) $(LIB))

##############################################################################
# Target
TARGET             := ozone
TARGET_H           := $(BUILD)$(TARGET).h
TARGET_LIB         := $(BUILD)lib$(TARGET).a
TARGET_DEBUG_LIB   :=  $(BUILD)lib$(TARGET).debug.a

##############################################################################
# Build
#
CFLAGS        := -Wall -Werror -Wextra -pedantic -O3 -I$(INCLUDE)
CLIBS         := -lm
OBJECTS       := $(patsubst $(SOURCE)%.c, $(BUILD)%.o, $(filter-out $(SOURCE)$(TARGET).c, $(wildcard *, $(SOURCE)*.c)))
DEBUG_OBJECTS := $(patsubst $(SOURCE)%.c, $(BUILD)%.debug.o, $(filter-out $(SOURCE)$(TARGET).c, $(wildcard *, $(SOURCE)*.c)))

$(BUILD)%.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -c $< $(CLIBS) -o $@

$(BUILD)%.debug.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -DOZONE_LOG_DEBUG -g -c $< $(CLIBS) -o $@

$(TARGET_H):
	$(COPY) $(INCLUDE)* $(BUILD)

$(TARGET_LIB): $(OBJECTS)
	$(ARCHIVER) $(TARGET_LIB) $(OBJECTS)

$(TARGET_DEBUG_LIB): $(DEBUG_OBJECTS)
	$(ARCHIVER) $(TARGET_DEBUG_LIB) $(DEBUG_OBJECTS)

build: $(TARGET_LIB) $(TARGET_H)
build-debug: $(TARGET_DEBUG_LIB) $(TARGET_H)

##############################################################################
# Testing
#
TEST_LIB_INCLUDES := $(LIB)Unity-2.6.0/src
TEST_LIB_SOURCES  := $(LIB)Unity-2.6.0/src
$(TEST_LIB_SOURCES):
	$(DOWNLOAD_TARBALL) https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.0.tar.gz | $(UNTAR_INTO) $(LIB)

TEST_LIB  := $(LIB)unity.o
$(TEST_LIB): $(TEST_LIB_SOURCES)
	$(CC) $(CFLAGS) -c $(LIB)Unity-2.6.0/src/unity.c $(CLIBS) -o $@

$(BUILD)%.test: $(TEST)%.test.c $(DEBUG_OBJECTS) $(TEST_LIB)
	$(CC) $(CFLAGS) $(foreach inc, $(TEST_LIB_INCLUDES), -I$(inc)) -g $^ $(CLIBS) -o $@
	$@

test: $(patsubst $(TEST)%.c, $(BUILD)%, $(wildcard *, $(TEST)*.test.c))

##############################################################################
# Examples
#
EXAMPLES    	  := $(ROOT)examples/
BUILD_EXAMPLES    := $(BUILD)examples/

$(BUILD_EXAMPLES)%: $(TARGET_LIB)
	$(shell $(MKDIR) $(BUILD_EXAMPLES))
	$(CC) $(CFLAGS) $(EXAMPLES)$*.c $< $(CLIBS) -o $@

$(BUILD_EXAMPLES)%.debug: $(TARGET_DEBUG_LIB)
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

all: build build-debug test

.PHONY: build build-debug test debug memcheck clean start all
