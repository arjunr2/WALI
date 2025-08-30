mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))

WALI_ROOT_DIR := $(patsubst %/,%, $(dir $(mkfile_path)))
WALI_BUILD_DIR := $(WALI_ROOT_DIR)/build

# ---- Build Artifact Directories ---- #
WALI_LLVM_DIR := $(WALI_BUILD_DIR)/llvm
WALI_SYSROOT_DIR := $(WALI_BUILD_DIR)/sysroot
WALI_LIBCXX_DIR := $(WALI_BUILD_DIR)/libcxx

# ---- Compiler/Linker Toolchain Binaries ---- #
WALI_LLVM_BIN_DIR := $(WALI_LLVM_DIR)/bin
WALI_CC := $(WALI_LLVM_BIN_DIR)/clang
WALI_CXX := $(WALI_LLVM_BIN_DIR)/clang++
WALI_LD := $(WALI_LLVM_BIN_DIR)/wasm-ld
WALI_AR := $(WALI_LLVM_BIN_DIR)/llvm-ar
WALI_RANLIB := $(WALI_LLVM_BIN_DIR)/llvm-ranlib

WALI_WARNING_CFLAGS := -Wno-implicit-function-declaration -Wno-int-conversion -Wno-incompatible-function-pointer-types
WALI_WASM_FEATURE_FLAGS := -mcpu=generic -matomics -mbulk-memory -mexception-handling
# ---- Common flags ---- #
WALI_COMMON_CFLAGS := -O0 --target=wasm32-wali-linux-musl -pthread $(WALI_WARNING_CFLAGS) --sysroot=$(WALI_SYSROOT_DIR) -fdeclspec -fwasm-exceptions $(WALI_WASM_FEATURE_FLAGS)
WALI_COMMON_CXXFLAGS := -stdlib=libc++ $(WALI_COMMON_CFLAGS) -I$(WALI_LIBCXX_DIR)/include/c++/v1
WALI_COMMON_LDFLAGS := -L$(WALI_SYSROOT_DIR)/lib -L$(WALI_LIBCXX_DIR)/lib -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=2147483648 -Wl,--undefined=__walirt_wasm_memory_size
