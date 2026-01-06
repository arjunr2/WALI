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

SRC_C := $(wildcard unit/test_*.c)
SRC_WASM := $(patsubst unit/%.c, $(WASM_DIR)/%.wasm, $(SRC_C))
SRC_ELF := $(patsubst unit/%.c, $(ELF_DIR)/%, $(SRC_C))

HOST_PLATFORM := $(shell $(WALI_LLVM_BIN_DIR)/llvm-config --host-target)

all: libnative_lib.so $(SRC_WASM) $(SRC_ELF)

libnative_lib.so: native_lib.c
	clang -I$(IWASM_ROOT)/core/iwasm/include -fPIC -shared -o $@ $< 

$(WASM_DIR)/%.wasm: unit/%.c
	@mkdir -p $(dir $@)
	$(WALI_CC) $(CFLAGS_TEST_WASM) -o $@ $<
	wasm2wat --enable-threads $@ -o $(@:.wasm=.wat)

$(ELF_DIR)/%: unit/%.c
	@mkdir -p $(dir $@)
	$(WALI_CC) --target=$(HOST_PLATFORM) $(CFLAGS_TEST_NATIVE) -o $@ $< -lm -lpthread

clean:
	rm -rf $(BIN_DIR) *.so

test: all
	python3 run_tests.py
