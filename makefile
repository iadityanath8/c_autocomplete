# ===============================
# Compiler
# ===============================

CC     := cc
TARGET := bin/app


# ===============================
# Flags
# ===============================

CSTD  := -std=c2x
WARN  := -Wall -Wextra -Wpedantic
OPT   := -O3
DEBUG := -g

CFLAGS  := $(CSTD) $(WARN) $(OPT) $(DEBUG)
LDFLAGS :=


# ===============================
# Files
# ===============================

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)


# ===============================
# Default
# ===============================

.PHONY: all
all: $(TARGET)


# ===============================
# Link
# ===============================

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)


# ===============================
# Compile
# ===============================

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# ===============================
# Clean
# ===============================

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)


# ===============================
# Debug Build
# ===============================

.PHONY: debug
debug: CFLAGS := $(CSTD) $(WARN) -O0 -g -DDEBUG
debug: clean all


# ===============================
# Run
# ===============================

.PHONY: run
run: all
	./$(TARGET)

