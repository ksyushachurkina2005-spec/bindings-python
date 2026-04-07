CC = gcc
CFLAGS = -Wall -Wextra -fPIC -I./include
LDFLAGS = -shared

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BINDINGS_DIR = bindings/python

# Target names
TARGET_LIB = libtaskdev.so

# Source files
LIB_SRCS = $(SRC_DIR)/taskdev.c $(SRC_DIR)/da.c
LIB_OBJS = $(LIB_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target
all: $(TARGET_LIB)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile library objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build shared library
$(TARGET_LIB): $(LIB_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Install library system-wide
install: $(TARGET_LIB)
	sudo cp $(TARGET_LIB) /usr/local/lib/
	sudo cp $(INC_DIR)/taskdev.h /usr/local/include/
	sudo ldconfig
	@echo "Library installed to /usr/local/lib/"

# Uninstall library
uninstall:
	sudo rm -f /usr/local/lib/$(TARGET_LIB)
	sudo rm -f /usr/local/include/taskdev.h
	sudo ldconfig
	@echo "Library removed from system"

# Build Python bindings
python-build:
	cd $(BINDINGS_DIR) && python3 build_taskdev.py

# Install Python module
python-install: python-build
	cd $(BINDINGS_DIR) && pip install .

# Run Python tests
test:
	cd $(BINDINGS_DIR) && python3 test_minimal.py

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET_LIB)
	rm -f $(BINDINGS_DIR)/_taskdev.*
	rm -rf $(BINDINGS_DIR)/__pycache__
	rm -rf $(BINDINGS_DIR)/build
	@echo "Clean complete"

# Full rebuild and test
rebuild: clean all python-build test

# Help
help:
	@echo "Available targets:"
	@echo "  make              - Build shared library"
	@echo "  make install      - Install library system-wide"
	@echo "  make uninstall    - Remove library from system"
	@echo "  make python-build - Build Python bindings"
	@echo "  make python-install - Install Python module"
	@echo "  make test         - Run Python tests"
	@echo "  make clean        - Remove build files"
	@echo "  make rebuild      - Clean, rebuild and test"

.PHONY: all install uninstall python-build python-install test clean rebuild help
