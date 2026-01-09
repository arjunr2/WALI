include ../toolchains/wali.mk

.PHONY: dir clean all test

BIN_DIR := bin
WASM_DIR := $(BIN_DIR)/unit/wasm
ELF_DIR := $(BIN_DIR)/unit/elf

# Flags
# We append our specific flags to the common ones
CFLAGS_TEST_WASM = $(WALI_COMMON_CFLAGS) $(WALI_COMMON_LDFLAGS) -I. -Icommon -nostartfiles -Wl,--no-entry -Wl,--allow-undefined
# For native, we need to include current dir for headers
CFLAGS_TEST_NATIVE = -I. -Icommon -O2

# Native lib flags
IWASM_ROOT = ../wasm-micro-runtime

SRC_C := $(wildcard unit/*.c)
# Filter out setup/cleanup files from main tests if they match *.c (usually named <test>.c vs <test>_setup.c)
# But wildcard picks everything. We can rely on pattern rules or filter.
# Assuming standard tests don't end in _setup.c or _cleanup.c? 
# Better: Explicitly look for them.

# We now generate hooks for ALL tests (using weak defaults if not present)
# But first, ignore any specific _hooks.c files if they still exist to avoid doubleness
SRC_TESTS := $(filter-out %_hooks.c, $(SRC_C))

SRC_WASM := $(patsubst unit/%.c, $(WASM_DIR)/%.wasm, $(SRC_TESTS))
SRC_ELF := $(patsubst unit/%.c, $(ELF_DIR)/%, $(SRC_TESTS))
BIN_HOOKS := $(patsubst unit/%.c, $(ELF_DIR)/%_hooks, $(SRC_TESTS))

HOST_PLATFORM := $(shell $(WALI_LLVM_BIN_DIR)/llvm-config --host-target)

all: libnative_lib.so $(SRC_WASM) $(SRC_ELF) $(BIN_HOOKS)

# ...

$(ELF_DIR)/%_hooks: unit/%.c common/hooks_main.c
	@mkdir -p $(dir $@)
	$(WALI_CC) --target=$(HOST_PLATFORM) $(CFLAGS_TEST_NATIVE) -DWALI_TEST_WRAPPER -o $@ common/hooks_main.c $<




libnative_lib.so: common/native_lib.c
	clang -I$(IWASM_ROOT)/core/iwasm/include -L$(WALI_BUILD_DIR)/wamr/iwasm/ -fPIC -shared -o $@ $< -lvmlib

$(WASM_DIR)/%.wasm: unit/%.c
	@mkdir -p $(dir $@)
	$(WALI_CC) $(CFLAGS_TEST_WASM) -o $@ $<
	wasm2wat --enable-threads $@ -o $(@:.wasm=.wat)

$(ELF_DIR)/%: unit/%.c
	@mkdir -p $(dir $@)
	$(WALI_CC) --target=$(HOST_PLATFORM) $(CFLAGS_TEST_NATIVE) -o $@ $< -lm -lpthread

clean:
	rm -rf $(BIN_DIR) *.so
