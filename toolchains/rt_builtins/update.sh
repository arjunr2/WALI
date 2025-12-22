#!/bin/bash
#
# Compile the WALI rt and bundle it into LLVM builtin
source $(dirname $(realpath ${BASH_SOURCE}))/../wali.sh

LLVM_VERSION=22
llvm_builtin=llvm-${LLVM_VERSION}.libclang_rt.builtins-wasm32-wali.a

$WALI_CC $WALI_COMMON_CFLAGS -c wali_rt.c -o wali_rt.o
$WALI_LLVM_BIN_DIR/llvm-ar rcs $llvm_builtin wali_rt.o
rm wali_rt.o
