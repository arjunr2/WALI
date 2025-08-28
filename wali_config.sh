#!/bin/bash

export WALI_ROOT_DIR=$(dirname $(realpath ${BASH_SOURCE}))
export WALI_BUILD_DIR=$WALI_ROOT_DIR/build

# Build Artifact Directories
export WALI_LLVM_DIR=$WALI_BUILD_DIR/llvm
export WALI_SYSROOT_DIR=$WALI_BUILD_DIR/sysroot
export WALI_LIBCXX_DIR=$WALI_BUILD_DIR/libcxx

# LLVM Compilation tools 
export WALI_LLVM_BIN_DIR=$WALI_LLVM_DIR/bin
export WALI_CC=$WALI_LLVM_BIN_DIR/clang
export WALI_CXX=$WALI_LLVM_BIN_DIR/clang++
export WALI_LD=$WALI_LLVM_BIN_DIR/wasm-ld
export WALI_AR=$WALI_LLVM_BIN_DIR/llvm-ar
export WALI_RANLIB=$WALI_LLVM_BIN_DIR/llvm-ranlib

# C/C++ flags
WASM_FEATURES="-mcpu=generic -matomics -mbulk-memory -mexception-handling"
export WALI_CFLAGS="--target=wasm32-wali-linux-musl -pthread --sysroot=$WALI_SYSROOT_DIR -fdeclspec -fwasm-exceptions $WASM_FEATURES"
export WALI_CXXFLAGS="$WALI_CFLAGS -stdlib=libc++ -I$WALI_ROOT_DIR/libcxx/include/c++/v1"
export WALI_LDFLAGS="-L$WALI_SYSROOT_DIR/lib -L$WALI_LIBCXX_DIR/lib -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=2147483648"

# Libclang RT
export WALI_LLVM_MAJOR_VERSION=$($WALI_LLVM_BIN_DIR/llvm-config --version | cut -d '.' -f 1)
export WALI_LIBCLANG_RT_LIB=$WALI_LLVM_DIR/lib/clang/$WALI_LLVM_MAJOR_VERSION/lib/linux/libclang_rt.builtins-wasm32.a

