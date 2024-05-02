mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))

ROOT_DIR := $(patsubst %/,%, $(dir $(mkfile_path)))
LLVM_DIR := $(ROOT_DIR)/llvm-project

# ---- Libc Sysroot ---- #
WALI_SYSROOT_DIR := $(ROOT_DIR)/wali-musl/sysroot

# ---- Compiler and Linker ---- #
WALI_LLVM_BIN_DIR := $(LLVM_DIR)/build/bin
WALI_CC := $(WALI_LLVM_BIN_DIR)/clang
WALI_CXX := $(WALI_LLVM_BIN_DIR)/clang++
WALI_LD := $(WALI_LLVM_BIN_DIR)/wasm-ld
WALI_AR := $(WALI_LLVM_BIN_DIR)/llvm-ar
WALI_RANLIB := $(WALI_LLVM_BIN_DIR)/llvm-ranlib

WALI_WARNING_CFLAGS := -Wno-implicit-function-declaration -Wno-int-conversion -Wno-incompatible-function-pointer-types
WALI_WASM_FEATURE_FLAGS := -matomics -mbulk-memory -mmutable-globals -msign-ext
# ---- Common flags ---- #
WALI_COMMON_CFLAGS := -O0 --target=wasm32-wasi-threads $(WALI_WARNING_CFLAGS) --sysroot=$(WALI_SYSROOT_DIR) -L$(WALI_SYSROOT_DIR)/lib $(WALI_WASM_FEATURE_FLAGS)
WALI_COMMON_LDFLAGS := -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=67108864

.PHONY: print-rootdir

print-rootdir:
	echo "Root dir: $(ROOT_DIR)"

