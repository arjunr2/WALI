#!/bin/bash

source ../wazi_config.sh

progbase=${1%.*}
outdir=wasms
mkdir -p $outdir

clang -v $WAZI_CFLAGS $progbase.c -c -o $outdir/$progbase.int.o
wasm-ld $WAZI_LDFLAGS $progbase.int.o -o $outdir/$progbase.wasm

wasm2wat --enable-threads $outdir/$progbase.wasm -o $outdir/$progbase.wat
#make -j6 CC=clang LD=wasm-ld MYCFLAGS="$cflags" MYLDFLAGS="$ldflags" AR="$WALI_AR rc" RANLIB=$WALI_RANLIB
#wasm2wat --enable-threads lua -o l.wat
#wat2wasm --enable-threads l.wat -o l.wasm
