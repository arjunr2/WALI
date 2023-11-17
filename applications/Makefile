.PHONY: all clean vim sqlite

CURDIR = $(shell pwd)

# ---- Sysroot ---- #
WALI_SYSROOT_DIR = $(CURDIR)/../wali-musl/sysroot
# ---- Compiler and Linker ---- #
WALI_LLVM_BIN_DIR = $(CURDIR)/../llvm-project/build/bin
WALI_CC = $(WALI_LLVM_BIN_DIR)/clang
WALI_LD = $(WALI_LLVM_BIN_DIR)/wasm-ld
WALI_AR = $(WALI_LLVM_BIN_DIR)/llvm-ar
WALI_RANLIB = $(WALI_LLVM_BIN_DIR)/llvm-ranlib


WALI_WARNING_CFLAGS = -Wno-implicit-function-declaration -Wno-int-conversion -Wno-incompatible-function-pointer-types
WALI_WASM_FEATURE_FLAGS = -matomics -mbulk-memory -mmutable-globals -msign-ext
# ---- Common flags ---- #
WALI_COMMON_CFLAGS = -O0 --target=wasm32-wasi-threads $(WALI_WARNING_CFLAGS) --sysroot=$(WALI_SYSROOT_DIR) -L$(WALI_SYSROOT_DIR)/lib $(WALI_WASM_FEATURE_FLAGS)
WALI_COMMON_LDFLAGS = -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=67108864

# --- Library path --- #
WALI_LIBS = $(CURDIR)/../libraries


all: vim sqlite

.ONESHELL:
vim:
	cd vim;
	make CC=$(WALI_CC) CClink="$(WALI_CC) $(WALI_COMMON_CFLAGS)" CFLAGS="$(WALI_COMMON_CFLAGS)" LDFLAGS="-L$(WALI_LIBS)/libncurses $(WALI_COMMON_LDFLAGS)" LD=$(WALI_LD) CONF_OPT_TERMINAL=--disable-terminal

sqlite:
	cd sqlite; mkdir -p build; cd build;
	../configure CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) BCC=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS) -U__wasi__" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	make

