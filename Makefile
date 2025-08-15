##############################################################################
# Toolchain
#
# Compiler
CC                := clang
EXTRA_CFLAGS      :=

# Debugger
DEBUGGER          := gdb

# Memory checking
MEMCHECK          := valgrind --leak-check=full -s

# Shell
CLEAN             := rm -rf
COPY              := cp -rf
MKDIR             := mkdir -p

# Download
DOWNLOAD_TARBALL  := curl -s -L
UNTAR_INTO        := tar xz -C

# C Formatter
FORMAT            := clang-format -i
FORMAT_CHECK      := clang-format -i -Werror --dry-run

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
# Platform
#
PLATFORM := $(shell uname -s)

ifeq ($(PLATFORM), Darwin)
PLATFORM_INCLUDES_PATH  := /usr/local/include/
PLATFORM_LIBS_PATH      := /usr/local/lib/
SHARED_OBJECT_EXTENSION := .dylib
else
PLATFORM_INCLUDES_PATH  := /usr/include/
PLATFORM_LIBS_PATH      := /usr/lib/
SHARED_OBJECT_EXTENSION := .so
endif

##############################################################################
# Target
#
TARGET             := ozone
TARGET_LIB         := $(BUILD)lib$(TARGET)$(SHARED_OBJECT_EXTENSION)
TARGET_DEBUG_LIB   := $(BUILD)lib$(TARGET).debug$(SHARED_OBJECT_EXTENSION)

##############################################################################
# C Build
#
CFLAGS_DEBUG  := -std=gnu99 -Wall -Werror -Wextra -pedantic -fpic -O0 -I$(INCLUDE) $(EXTRA_CFLAGS) -DOZONE_LOG_DEBUG -g
CFLAGS        := -std=gnu99 -Wall -Werror -Wextra -pedantic -fpic -O3 -I$(INCLUDE) $(EXTRA_CFLAGS)

$(TARGET_LIB):
	$(CC) -shared $(CFLAGS) -o $(TARGET_LIB) $(SOURCE)$(TARGET).c

build: $(TARGET_LIB)

$(TARGET_DEBUG_LIB):
	$(CC) -shared -DOZONE_LOG_DEBUG -g $(CFLAGS_DEBUG) -o $(TARGET_DEBUG_LIB) $(SOURCE)$(TARGET).c

build-debug: $(TARGET_DEBUG_LIB)

##############################################################################
# Examples
#
EXAMPLES    	  := $(ROOT)examples/
BUILD_EXAMPLES    := $(BUILD)examples/

$(BUILD_EXAMPLES)%.debug: $(TARGET_DEBUG_LIB)
	$(shell $(MKDIR) $(BUILD_EXAMPLES)) $(CC) -I$(EXAMPLES) $(CFLAGS_DEBUG) $(EXAMPLES)$*.c $(TARGET_DEBUG_LIB) -o $@

$(BUILD_EXAMPLES)%: $(TARGET_LIB)
	$(shell $(MKDIR) $(BUILD_EXAMPLES))	$(CC) -I$(EXAMPLES) $(CFLAGS) $(EXAMPLES)$*.c $(TARGET_LIB) -o $@

%.memcheck: $(BUILD_EXAMPLES)%.debug
	$(MEMCHECK) $(BUILD_EXAMPLES)$*.debug

build-examples: $(patsubst $(EXAMPLES)%.c, $(BUILD_EXAMPLES)%, $(wildcard *, $(EXAMPLES)*.c))
build-examples-debug: $(patsubst $(EXAMPLES)%.c, $(BUILD_EXAMPLES)%.debug, $(wildcard *, $(EXAMPLES)*.c))

EXAMPLES_ARGS     := --serve-directory=$(EXAMPLES)assets --content-types=$(EXAMPLES)configuration/content_types.properties
%: $(BUILD_EXAMPLES)%
	$(BUILD_EXAMPLES)$* $(EXAMPLES_ARGS)

%.debug: $(BUILD_EXAMPLES)%.debug
	$(DEBUGGER) --args $(BUILD_EXAMPLES)$*.debug $(EXAMPLES_ARGS)

##############################################################################
# Testing
#
TEST_LIB_SOURCE  := $(LIB)Unity-2.6.0/src/
$(TEST_LIB_SOURCE):
	$(DOWNLOAD_TARBALL) https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.0.tar.gz | $(UNTAR_INTO) $(LIB)

TEST_LIB  := $(LIB)unity.o
$(TEST_LIB): $(TEST_LIB_SOURCE)
	$(CC) $(CFLAGS) -c $(TEST_LIB_SOURCE)unity.c -o $@

$(BUILD)%.test: $(TEST)%.test.c $(TARGET_DEBUG_LIB) $(TEST_LIB)
	$(CC) $(CFLAGS) -I$(TEST_LIB_SOURCE) -g $^ -o $@
	$@

test: $(patsubst $(TEST)%.c, $(BUILD)%, $(wildcard *, $(TEST)*.test.c))

BENCHMARKS  := $(TEST)benchmarks/
%.benchmarks: $(BUILD_EXAMPLES)%
	cd $(TEST)benchmarks && deno install && deno task run $^ | tee $(BENCHMARKS)$@_$(PLATFORM).md

##############################################################################
# Installation
#
install: build uninstall
	$(COPY) $(INCLUDE) $(PLATFORM_INCLUDES_PATH)$(TARGET) \
	  && $(COPY) $(TARGET_LIB) $(PLATFORM_LIBS_PATH)

uninstall:
	$(CLEAN) $(PLATFORM_INCLUDES_PATH)$(TARGET) \
	  && $(CLEAN) $(PLATFORM_LIBS_PATH)$(TARGET_LIB)

##############################################################################
# Helpers
#
FORMAT_TARGETS := **/*.c **/*.h

format:
	cd $(ROOT) && $(FORMAT) $(FORMAT_TARGETS)

format-check:
	cd $(ROOT) && $(FORMAT_CHECK) $(FORMAT_TARGETS)

clean:
	$(CLEAN) $(BUILD)

all: build build-debug build-examples test

.DELETE_ON_ERROR:
.PHONY: Makefile format format-check build build-debug build-examples build-examples-debug test benchmarks clean install uninstall all
