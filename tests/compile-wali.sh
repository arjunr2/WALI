#!/bin/bash

outdir=.
LLVM_DIR=../llvm-project

rt_lib=$LLVM_DIR/build/lib/clang/16/lib/wasi/libclang_rt.builtins-wasm32.a

CC=$LLVM_DIR/build/bin/clang
LD=$LLVM_DIR/build/bin/wasm-ld

while getopts "vo:s:" OPT; do
  case $OPT in
    v) verbose=--verbose;;
    o) outdir=$OPTARG;;
    s) sysroot_dir=$OPTARG;;
    *) 
      echo "Incorrect opt provided"
      exit 1 ;;
  esac
done
cfile=${@:$OPTIND:1}
outbase=$outdir/$(basename $cfile .c)

crtfile=$sysroot_dir/lib/crt1.o 

$CC $verbose --target=wasm32-wasi-threads -O0 -pthread --sysroot=$sysroot_dir $cfile -c -o $outbase.wasm
wasm2wat --enable-threads $outbase.wasm -o $outbase.wat
$LD $verbose --no-gc-sections --no-entry --shared-memory --export-memory --import-memory --max-memory=67108864 --allow-undefined -L$sysroot_dir/lib $outbase.wasm $crtfile -lc -lm $rt_lib -o ${outbase}_link.wasm
wasm2wat --enable-threads ${outbase}_link.wasm -o ${outbase}_link.wat
