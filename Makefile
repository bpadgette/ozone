##############################################################################
# Toolchain
#
CC                := gcc
CLEAN             := rm -rf
COPY              := cp -rf
DOWNLOAD_TARBALL  := curl -s -L
FORMAT            := clang-format -i
FORMAT_CHECK      := clang-format -i -Werror --dry-run
FORMAT_TARGETS    := **/*.c **/*.h
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
CFLAGS        := -std=gnu99 -Wall -Werror -Wextra -pedantic -fpic -O3 -I$(INCLUDE)
CLIBS         := -lm
OBJECTS       := $(patsubst $(SOURCE)%.c, $(BUILD)%.o, $(wildcard *, $(SOURCE)*.c))
DEBUG_OBJECTS := $(patsubst $(SOURCE)%.c, $(BUILD)%.debug.o, $(wildcard *, $(SOURCE)*.c))

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

build-examples: $(patsubst $(EXAMPLES)%.c, $(BUILD_EXAMPLES)%, $(wildcard *, $(EXAMPLES)*.c))

ex-%: $(BUILD_EXAMPLES)%
	$(BUILD_EXAMPLES)$*

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
format:
	cd $(ROOT) && $(FORMAT) $(FORMAT_TARGETS)

format-check:
	cd $(ROOT) && $(FORMAT_CHECK) $(FORMAT_TARGETS)

clean:
	$(CLEAN) $(BUILD)

all: build build-debug build-examples test

.DELETE_ON_ERROR:
.PHONY: format format-check build build-debug build-examples test clean install uninstall all
