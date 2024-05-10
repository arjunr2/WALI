include ../wali_config.mk

.PHONY: dir clean all

# For ELF
ELF_LFLAGS=-lm -lpthread

WASM_DIR=wasm
ELF_DIR=elf

SRC_C:=$(wildcard *.c)
SRC_O:=$(SRC_C:.c=.o)
SRC_WASM:=$(SRC_C.c=.wasm)

all: dir $(SRC_O)

dir:
	mkdir -p $(WASM_DIR)
	mkdir -p $(ELF_DIR)

%.o: %.c
	$(WALI_CC) -O1 -o $(ELF_DIR)/$* $< $(ELF_LFLAGS)
	./compile-wali.sh -o $(WASM_DIR) -s $(WALI_SYSROOT_DIR) $<


clean:
	rm -r $(WASM_DIR)
	rm -r $(ELF_DIR)
