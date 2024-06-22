#!/bin/bash

source $(dirname $(realpath ${BASH_SOURCE}))/../wali_config.sh

LVERSION=$($WALI_LLVM_BIN_DIR/llvm-config --version | cut -d '.' -f 1)
LIBCLANG_RT=$WALI_LLVM_DIR/build/lib/clang/$LVERSION/lib/libclang_rt.builtins-wasm32.a

export WAZI_SYSROOT_DIR=$WALI_ROOT_DIR/picolibc/install/picolibc/wasm32-wasi-threads
export WAZI_CFLAGS="--sysroot=$WAZI_SYSROOT_DIR $WASM_FEATURES -isystem $WAZI_SYSROOT_DIR/include/ -isystem $WAZI_SYSROOT_DIR/build -fno-stack-protector -fno-common"
export WAZI_LDFLAGS="--export=__heap_base --export=__data_end --export=__heap_end --allow-undefined --shared-memory --export-memory --max-memory=131072 --warn-unresolved-symbols -L$WAZI_SYSROOT_DIR/lib $WAZI_SYSROOT_DIR/lib/crt0.o $LIBCLANG_RT -lc -lm"
