include ../../toolchains/wali.mk

.PHONY: dir clean all elf

# For ELF
ELF_LFLAGS := -lm -lpthread

WASM_DIR := wasm
ELF_DIR := elf

SRC_C := $(wildcard *.c)
# Generate ELF files only on linux platforms
ifeq ($(UNAME_S),Linux)
	SRC_O := $(SRC_C:.c=.o)
endif

SRC_WASM := $(SRC_C:.c=.wasm)

HOST_PLATFORM := $(shell $(WALI_LLVM_BIN_DIR)/llvm-config --host-target)

all: dir $(SRC_WASM) $(SRC_O)

dir:
	mkdir -p $(WASM_DIR)
	mkdir -p $(ELF_DIR)

%.o: %.c
	$(WALI_CC) --target=$(HOST_PLATFORM) -O1 -o $(ELF_DIR)/$* $< $(ELF_LFLAGS)

%.wasm: %.c
	$(WALI_CC) $(WALI_COMMON_CFLAGS) $(WALI_COMMON_LDFLAGS) -o $(WASM_DIR)/$*.wasm $<
	wasm2wat --enable-threads $(WASM_DIR)/$*.wasm -o $(WASM_DIR)/$*.wat


clean:
	rm -rf $(WASM_DIR)
	rm -rf $(ELF_DIR)

