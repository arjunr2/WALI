#!/bin/bash
export WALI_DIR=$(dirname $(realpath ${BASH_SOURCE}))

WALI_LLVM_BIN=$WALI_DIR/llvm-project/build/bin

# LLVM Compilation tools 
export WALI_CC=$WALI_LLVM_BIN/clang
export WALI_CXX=$WALI_LLVM_BIN/clang++
export WALI_LD=$WALI_LLVM_BIN/wasm-ld
export WALI_AR=$WALI_LLVM_BIN/llvm-ar
export WALI_RANLIB=$WALI_LLVM_BIN/llvm-ranlib

# Sysroot
export WALI_SYSROOT_DIR=$WALI_DIR/wali-musl/sysroot

# C/C++ flags
export WALI_CFLAGS="--target=wasm32-wasi-threads -pthread --sysroot=/$WALI_DIR/wali-musl/sysroot -matomics -mbulk-memory -mmutable-globals -msign-ext"
export WALI_CXXFLAGS="-stdlib=libc++ --target=wasm32-wasi-threads -pthread --sysroot=/$WALI_DIR/wali-musl/sysroot -I$WALI_DIR/libcxx/include/c++/v1 -matomics -mbulk-memory -mmutable-globals -msign-ext"
export WALI_LDFLAGS="-L/$WALI_DIR/wali-musl/sysroot/lib -L/$WALI_DIR/libcxx/lib -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=2147483648"

