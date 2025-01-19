##############################################################################
# Toolchain
#
# Compiler
CC                := clang

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
JS_PROJECT  := $(ROOT)ozone_js/
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
TARGET_JS_MODULE   := $(BUILD)$(TARGET).js

##############################################################################
# JS Build
#
$(TARGET_JS_MODULE):
	cd $(JS_PROJECT) && deno install && deno task build

build-js: $(TARGET_JS_MODULE)

##############################################################################
# C Build
#
CFLAGS        := -std=gnu99 -Wall -Werror -Wextra -pedantic -fpic -O3 -I$(INCLUDE)
OBJECTS       := $(patsubst $(SOURCE)%.c, $(BUILD)%.o, $(wildcard *, $(SOURCE)*.c))
DEBUG_OBJECTS := $(patsubst $(SOURCE)%.c, $(BUILD)%.debug.o, $(wildcard *, $(SOURCE)*.c))

$(BUILD)%.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)%.debug.o: $(SOURCE)%.c
	$(CC) $(CFLAGS) -DOZONE_LOG_DEBUG -g -c $< -o $@

$(TARGET_LIB): $(OBJECTS)
	$(CC) -shared -o $(TARGET_LIB) $^

$(TARGET_DEBUG_LIB): $(DEBUG_OBJECTS)
	$(CC) -shared -o $(TARGET_DEBUG_LIB) $^

build: $(TARGET_LIB) $(TARGET_JS_MODULE)
build-debug: $(TARGET_DEBUG_LIB) $(TARGET_JS_MODULE)

##############################################################################
# Testing
#
TEST_LIB_SOURCE  := $(LIB)Unity-2.6.0/src/
$(TEST_LIB_SOURCE):
	$(DOWNLOAD_TARBALL) https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.0.tar.gz | $(UNTAR_INTO) $(LIB)

TEST_LIB  := $(LIB)unity.o
$(TEST_LIB): $(TEST_LIB_SOURCE)
	$(CC) $(CFLAGS) -c $(TEST_LIB_SOURCE)unity.c -o $@

$(BUILD)%.test: $(TEST)%.test.c $(DEBUG_OBJECTS) $(TEST_LIB)
	$(CC) $(CFLAGS) -I$(TEST_LIB_SOURCE) -g $^ -o $@
	$@

test: $(patsubst $(TEST)%.c, $(BUILD)%, $(wildcard *, $(TEST)*.test.c))

##############################################################################
# Examples
#
EXAMPLES    	  := $(ROOT)examples/
BUILD_EXAMPLES    := $(BUILD)examples/

$(BUILD_EXAMPLES)%: $(TARGET_LIB) $(TARGET_JS_MODULE)
	$(shell $(MKDIR) $(BUILD_EXAMPLES))	$(CC) $(CFLAGS) $(EXAMPLES)$*.c $< -o $@

$(BUILD_EXAMPLES)%.debug: $(TARGET_DEBUG_LIB) $(TARGET_JS_MODULE)
	$(shell $(MKDIR) $(BUILD_EXAMPLES)) $(CC) $(CFLAGS) -DOZONE_LOG_DEBUG -g $(EXAMPLES)$*.c $< -o $@

%.memcheck: $(BUILD_EXAMPLES)%.debug
	$(MEMCHECK) $(BUILD_EXAMPLES)$*.debug

build-examples: $(patsubst $(EXAMPLES)%.c, $(BUILD_EXAMPLES)%, $(wildcard *, $(EXAMPLES)*.c))
build-examples-debug: $(patsubst $(EXAMPLES)%.c, $(BUILD_EXAMPLES)%.debug, $(wildcard *, $(EXAMPLES)*.c))

%: $(BUILD_EXAMPLES)%
	$(BUILD_EXAMPLES)$*

##############################################################################
# Installation
#
install: build uninstall
	sudo $(COPY) $(INCLUDE) /usr/include/$(TARGET) \
	  && sudo $(COPY) $(TARGET_JS_MODULE) /usr/include/$(TARGET)/$(TARGET).js \
	  && sudo $(COPY) $(TARGET_LIB) /usr/lib

uninstall:
	sudo $(CLEAN) /usr/include/$(TARGET) &&	sudo $(CLEAN) /usr/lib/lib$(TARGET).so

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
.PHONY: Makefile format format-check build build-debug build-examples build-examples-debug build-js test clean install uninstall all
