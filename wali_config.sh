#!/bin/bash

export WALI_ROOT_DIR=$(dirname $(realpath ${BASH_SOURCE}))
export WALI_LLVM_DIR=$WALI_ROOT_DIR/llvm-project

# Sysroot
export WALI_SYSROOT_DIR=$WALI_ROOT_DIR/wali-musl/sysroot
export WALI_LIBCXX_DIR=$WALI_ROOT_DIR/libcxx

# LLVM Compilation tools 
export WALI_LLVM_BIN_DIR=$WALI_ROOT_DIR/llvm-project/build/bin
export WALI_CC=$WALI_LLVM_BIN_DIR/clang
export WALI_CXX=$WALI_LLVM_BIN_DIR/clang++
export WALI_LD=$WALI_LLVM_BIN_DIR/wasm-ld
export WALI_AR=$WALI_LLVM_BIN_DIR/llvm-ar
export WALI_RANLIB=$WALI_LLVM_BIN_DIR/llvm-ranlib

# C/C++ flags
export WASM_FEATURES="-matomics -mbulk-memory -mmutable-globals -msign-ext -mexception-handling"
export WALI_CFLAGS="--target=wasm32-wasi-threads -pthread --sysroot=$WALI_SYSROOT_DIR -fdeclspec $WASM_FEATURES"
export WALI_CXXFLAGS="$WALI_CFLAGS -stdlib=libc++ -I$WALI_ROOT_DIR/libcxx/include/c++/v1"
export WALI_LDFLAGS="-L$WALI_SYSROOT_DIR/lib -L$WALI_LIBCXX_DIR/lib -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=262144"

# Libclang RT
export WALI_LLVM_MAJOR_VERSION=$($WALI_LLVM_BIN_DIR/llvm-config --version | cut -d '.' -f 1)
export WALI_LIBCLANG_RT_LIB=$WALI_ROOT_DIR/llvm-project/build/lib/clang/$WALI_LLVM_MAJOR_VERSION/lib/wasi/libclang_rt.builtins-wasm32.a

