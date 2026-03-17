CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Wpedantic -O2 -Iinclude -fPIC
LDFLAGS = -shared

# Installation directories (override with e.g., make install PREFIX=/usr)
PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include

# Library path for linking and running tests (default = build directory)
LIBPATH ?= build

SRC = src/taskdev.c
OBJ = build/taskdev.o
LIB = build/libtaskdev.so
HEADERS = include/taskdev.h include/taskdev.hpp

TEST_C = tests/c/1_grading.c
TEST_CXX = tests/cpp/1_grading.cpp
TEST_EXECS = $(TEST_C:.c=) $(TEST_CXX:.cpp=)

.PHONY: all clean tests check install uninstall

all: $(LIB)

$(OBJ): $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ)
	$(CC) $(LDFLAGS) $< -o $@

tests: $(LIB) $(TEST_EXECS)

$(TEST_C:.c=): %: %.c
	$(CC) $< -Iinclude -Lbuild -ltaskdev -o $@

$(TEST_CXX:.cpp=): %: %.cpp
	$(CXX) $< -Iinclude -Lbuild -ltaskdev -o $@

run: tests
	@echo "Running tests..."
	@for exe in $(TEST_EXECS); do \
		echo "Running $$exe"; \
		LD_LIBRARY_PATH=$(LIBPATH) $$exe; \
	done

install: $(LIB) $(HEADERS)
	install -d $(DESTDIR)$(LIBDIR) $(DESTDIR)$(INCLUDEDIR)
	install -m 755 $(LIB) $(DESTDIR)$(LIBDIR)
	install -m 644 $(HEADERS) $(DESTDIR)$(INCLUDEDIR)

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/$(notdir $(LIB))
	for h in $(HEADERS); do \
		rm -f $(DESTDIR)$(INCLUDEDIR)/$$(basename $$h); \
	done

clean:
	rm -rf build $(TEST_EXECS)
