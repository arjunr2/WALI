include ../wali_config.mk

.PHONY: dir clean all

# For ELF
ELF_LFLAGS=-lm -lpthread

WASM_DIR=wasm
ELF_DIR=elf

SRC_C:=$(wildcard *.c)
SRC_O:=$(SRC_C:.c=.o)
SRC_WASM:=$(SRC_C.c=.wasm)

HOST_PLATFORM:=$(shell $(WALI_LLVM_BIN_DIR)/llvm-config --host-target)

all: dir $(SRC_O)

dir:
	mkdir -p $(WASM_DIR)
	mkdir -p $(ELF_DIR)

%.o: %.c
	$(WALI_CC) --target=$(HOST_PLATFORM) -O1 -o $(ELF_DIR)/$* $< $(ELF_LFLAGS)
	./compile-wali.sh -o $(WASM_DIR) -s $(WALI_SYSROOT_DIR) $<


clean:
	rm -rf $(WASM_DIR)
	rm -rf $(ELF_DIR)

