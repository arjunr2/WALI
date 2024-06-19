mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))

WALI_ROOT_DIR := $(patsubst %/,%, $(dir $(mkfile_path)))
WALI_LLVM_DIR := $(WALI_ROOT_DIR)/llvm-project

# ---- Libc Sysroot ---- #
WALI_SYSROOT_DIR := $(WALI_ROOT_DIR)/wali-musl/sysroot
WALI_LIBCXX_DIR := $(WALI_ROOT_DIR)/libcxx

# ---- Compiler and Linker ---- #
WALI_LLVM_BIN_DIR := $(WALI_LLVM_DIR)/build/bin
WALI_CC := $(WALI_LLVM_BIN_DIR)/clang
WALI_CXX := $(WALI_LLVM_BIN_DIR)/clang++
WALI_LD := $(WALI_LLVM_BIN_DIR)/wasm-ld
WALI_AR := $(WALI_LLVM_BIN_DIR)/llvm-ar
WALI_RANLIB := $(WALI_LLVM_BIN_DIR)/llvm-ranlib

WALI_WARNING_CFLAGS := -Wno-implicit-function-declaration -Wno-int-conversion -Wno-incompatible-function-pointer-types
WALI_WASM_FEATURE_FLAGS := -matomics -mbulk-memory -mmutable-globals -msign-ext -mexception-handling
# ---- Common flags ---- #
WALI_COMMON_CFLAGS := -O0 --target=wasm32-wasi-threads -pthread $(WALI_WARNING_CFLAGS) --sysroot=$(WALI_SYSROOT_DIR) -fwasm-exceptions -fdeclspec $(WALI_WASM_FEATURE_FLAGS)
WALI_COMMON_CXXFLAGS := -stdlib=libc++ $(WALI_COMMON_CFLAGS) -I$(WALI_ROOT_DIR)/libcxx/include/c++/v1
WALI_COMMON_LDFLAGS := -L$(WALI_SYSROOT_DIR)/lib -L$(WALI_LIBCXX_DIR)/lib -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=2147483648
