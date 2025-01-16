# =============================================================================
# Makefile for Object Pool Library with CLI Logger
# =============================================================================

# -------------------------------
# Variables
# -------------------------------

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c11 -Iinclude
CFLAGS_STATIC := $(CFLAGS) -fPIC
CFLAGS_SHARED := $(CFLAGS) -fPIC -DBUILDING_DLL
LDFLAGS := -pthread
LDLIBS := -pthread

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
BIN_DIR := bin
LIB_DIR := lib
TESTS_DIR := tests
DEPS_DIR := $(BUILD_DIR)/deps

# Source and Object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Test files
TEST_SRCS := $(wildcard $(TESTS_DIR)/*.c)
TEST_OBJS := $(patsubst $(TESTS_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))
TEST_BINARIES := $(patsubst $(TESTS_DIR)/%.c, $(BIN_DIR)/%, $(TEST_SRCS))

# Libraries
LIB_STATIC := $(LIB_DIR)/libobject_pool.a
LIB_SHARED := $(LIB_DIR)/libobject_pool.so

# Archiver
AR := ar
ARFLAGS := rcs

# Linker for shared library
CC_SHARED := $(CC)
LDFLAGS_SHARED := -shared

# Compiler flags for dependency generation
DEPFLAGS = -MMD -MP -MF $(DEPS_DIR)/$*.d

# -------------------------------
# Targets
# -------------------------------

.PHONY: all build static shared tests tests_build run_test install uninstall

# Default target
all: build $(LIB_STATIC) $(LIB_SHARED) tests_build

# Create build directories (order-only prerequisites)
build: $(DEPS_DIR)

$(DEPS_DIR):
	@mkdir -p $(BUILD_DIR) $(BIN_DIR) $(LIB_DIR) $(DEPS_DIR)

# Compile source object files with dependencies
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | build
	@echo "[CC] Compiling $< -> $@"
	@$(CC) $(CFLAGS_STATIC) $(DEPFLAGS) -c $< -o $@

# Create static library
$(LIB_STATIC): $(OBJS)
	@echo "[AR] Creating static library $@"
	@$(AR) $(ARFLAGS) $@ $^

# Create shared library
$(LIB_SHARED): $(OBJS)
	@echo "[LD] Creating shared library $@"
	@$(CC_SHARED) $(CFLAGS_SHARED) $(LDFLAGS_SHARED) -o $@ $^

# Compile and link all test binaries
tests_build: $(TEST_BINARIES)

$(BIN_DIR)/%: $(BUILD_DIR)/%.o $(LIB_STATIC)
	@echo "[LD] Linking test binary $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(TESTS_DIR)/%.c | build
	@echo "[CC] Compiling $< -> $@"
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Run all tests
tests: tests_build
	@echo "[TEST] Running all tests..."
	@for test in $(TEST_BINARIES); do \
		echo "[RUN] Running $$test"; \
		./$$test; \
	done

# Run a specific test
run_test:
	@if [ -z "$(TEST)" ]; then \
		echo "Please specify a test to run. Usage: make run_test TEST=test_name"; \
		exit 1; \
	fi
	@if [ ! -f "$(BIN_DIR)/$(TEST)" ]; then \
		echo "Test '$(TEST)' does not exist."; \
		exit 1; \
	fi
	@echo "[TEST] Running test $(TEST)"
	@./$(BIN_DIR)/$(TEST)

# Build static library only
static: build $(LIB_STATIC)

# Build shared library only
shared: build $(LIB_SHARED)

# Clean build artifacts
clean:
	@echo "[CLEAN] Removing build and binary artifacts..."
	@rm -rf $(BUILD_DIR)/* $(BIN_DIR)/*

# Full clean (including libraries)
fclean: clean
	@echo "[FCLEAN] Removing library files..."
	@rm -rf $(LIB_DIR)/*

# Rebuild the project
re: fclean all

# Help target
help:
	@echo "Object Pool Library with CLI Logger Makefile"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all         Build static and shared libraries, and all test binaries"
	@echo "  build       Create build, bin, lib, and deps directories"
	@echo "  static      Build the static library (libobject_pool.a)"
	@echo "  shared      Build the shared library (libobject_pool.so)"
	@echo "  tests_build Build all test binaries"
	@echo "  tests       Build and run all tests"
	@echo "  run_test    Build and run a specific test (requires TEST=test_name)"
	@echo "  install     Install the library and headers to system directories"
	@echo "  uninstall   Uninstall the library and headers from system directories"
	@echo "  clean       Remove build and binary artifacts"
	@echo "  fclean      Remove all build artifacts and libraries"
	@echo "  re          Rebuild the entire project from scratch"
	@echo "  help        Display this help message"

# -------------------------------
# Installation Targets
# -------------------------------

install: all
	@echo "Installing headers..."
	@mkdir -p /usr/local/include/object_pool_library
	@cp $(INCLUDE_DIR)/*.h /usr/local/include/object_pool_library/

	@echo "Installing libraries..."
	@cp $(LIB_DIR)/$(LIB_STATIC) /usr/local/lib/
	@cp $(LIB_DIR)/$(LIB_SHARED) /usr/local/lib/

	@echo "Creating symbolic links for shared library..."
	@ln -sf /usr/local/lib/$(LIB_SHARED) /usr/local/lib/libobject_pool.so.1
	@ln -sf /usr/local/lib/libobject_pool.so.1 /usr/local/lib/libobject_pool.so

	@echo "Installing pkg-config file..."
	@mkdir -p $(PKGCONFIG_DIR)
	@cp object_pool.pc $(PKGCONFIG_DIR)/

	@echo "Installation completed."

uninstall:
	@echo "Uninstalling libraries..."
	@rm -f /usr/local/lib/$(LIB_STATIC) /usr/local/lib/$(LIB_SHARED) /usr/local/lib/libobject_pool.so /usr/local/lib/libobject_pool.so.1

	@echo "Uninstalling headers..."
	@rm -rf /usr/local/include/object_pool_library

	@echo "Uninstallation completed."

# -------------------------------
# Dependencies
# -------------------------------

# Automatically include dependency files from deps directory
-include $(patsubst $(BUILD_DIR)/%.o, $(DEPS_DIR)/%.d, $(OBJS) $(TEST_OBJS))
