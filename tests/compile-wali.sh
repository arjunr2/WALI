#!/bin/bash

crtfile=startup/crt1.o 

outdir=.

while getopts "vo:s:" OPT; do
  case $OPT in
    v) verbose=-v;;
    o) outdir=$OPTARG;;
    s) sysroot_dir=$OPTARG;;
    *) 
      echo "Incorrect opt provided"
      exit 1 ;;
  esac
done
cfile=${@:$OPTIND:1}
outbase=$outdir/$(basename $cfile .c)

clang --target=wasm32 --sysroot=$sysroot_dir $cfile -S -emit-llvm -o $outbase.ll $verbose
llc $outbase.ll -filetype=obj -o $outbase.wasm
wasm2wat $outbase.wasm -o $outbase.wat
wasm-ld --no-entry --allow-undefined -L$sysroot_dir/lib $outbase.wasm $crtfile -lc -o ${outbase}_link.wasm
wasm2wat ${outbase}_link.wasm -o ${outbase}_link.wat