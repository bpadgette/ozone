##############################################################################
# Toolchain
#
CC                := gcc
CLEAN             := rm -rf
COPY              := cp -rf
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
#
TARGET             := ozone
TARGET_LIB         := $(BUILD)lib$(TARGET).so
TARGET_DEBUG_LIB   := $(BUILD)lib$(TARGET).debug.so

##############################################################################
# Build
#
CFLAGS        := -Wall -Werror -Wextra -pedantic -fpic -O3 -I$(INCLUDE)
CLIBS         := -lm
OBJECTS       := $(patsubst $(SOURCE)%.c, $(BUILD)%.o, $(filter-out $(SOURCE)$(TARGET).c, $(wildcard *, $(SOURCE)*.c)))
DEBUG_OBJECTS := $(patsubst $(SOURCE)%.c, $(BUILD)%.debug.o, $(filter-out $(SOURCE)$(TARGET).c, $(wildcard *, $(SOURCE)*.c)))

$(BUILD)%.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -c $< $(CLIBS) -o $@

$(BUILD)%.debug.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -DOZONE_LOG_DEBUG -g -c $< $(CLIBS) -o $@

$(TARGET_LIB): $(OBJECTS)
	$(CC) -shared -o $(TARGET_LIB) $^

$(TARGET_DEBUG_LIB): $(DEBUG_OBJECTS)
	$(CC) -shared -o $(TARGET_DEBUG_LIB) $^

build: $(TARGET_LIB)
build-debug: $(TARGET_DEBUG_LIB)

##############################################################################
# Testing
#
TEST_LIB_INCLUDES := $(LIB)Unity-2.6.0/src/
TEST_LIB_SOURCE  := $(LIB)Unity-2.6.0/src/
$(TEST_LIB_SOURCE):
	$(DOWNLOAD_TARBALL) https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.0.tar.gz | $(UNTAR_INTO) $(LIB)

TEST_LIB  := $(LIB)unity.o
$(TEST_LIB): $(TEST_LIB_SOURCE)
	$(CC) $(CFLAGS) -c $(TEST_LIB_SOURCE)unity.c $(CLIBS) -o $@

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
	$(shell $(MKDIR) $(BUILD_EXAMPLES))	$(CC) $(CFLAGS) $(EXAMPLES)$*.c $< $(CLIBS) -o $@

$(BUILD_EXAMPLES)%.debug: $(TARGET_DEBUG_LIB)
	$(shell $(MKDIR) $(BUILD_EXAMPLES)) && $(CC) $(CFLAGS) -DOZONE_LOG_DEBUG -g $(EXAMPLES)$*.c $< $(CLIBS) -o $@

start: $(BUILD_EXAMPLES)start
	$<

debug: $(BUILD_EXAMPLES)start.debug
	$(DEBUGGER) $<

memcheck: $(BUILD_EXAMPLES)start.debug
	$(MEMCHECK) $<

##############################################################################
# Installation
#
install: build uninstall
	sudo $(COPY) $(INCLUDE) /usr/include/$(TARGET) && sudo $(COPY) $(TARGET_LIB) /usr/lib

uninstall:
	sudo $(CLEAN) /usr/include/$(TARGET) &&	sudo $(CLEAN) /usr/lib/lib$(TARGET).so

##############################################################################
# Helpers
#
clean:
	$(CLEAN) $(BUILD)

all: build build-debug test

.PHONY: build build-debug test debug memcheck clean start install uninstall all
