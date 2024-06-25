#!/bin/bash

source ../wazi_config.sh

progbase=${1%.*}
outdir=wasms
mkdir -p $outdir

clang $WAZI_CFLAGS $progbase.c -c -o $outdir/$progbase.int.o
wasm-ld $WAZI_LDFLAGS $outdir/$progbase.int.o -o $outdir/$progbase.wasm

wasm2wat --enable-threads $outdir/$progbase.wasm -o $outdir/$progbase.wat
