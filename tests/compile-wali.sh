#!/bin/bash

outdir=.
rt_lib=/home/arjun/tools/llvm-project/build/lib/clang/16/lib/wasi/libclang_rt.builtins-wasm32.a

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

crtfile=$sysroot_dir/lib/crt1.o 

#clang --target=wasm32-wasi-threads -O1 -pthread --sysroot=$sysroot_dir $cfile -S -emit-llvm -o $outbase.ll $verbose
#llc $outbase.ll -filetype=obj -o $outbase.wasm
#wasm2wat $outbase.wasm -o $outbase.wat
#wasm-ld --no-entry --shared-memory --allow-undefined -L$sysroot_dir/lib $outbase.wasm $crtfile -lc -lm $rt_lib -o ${outbase}_link.wasm
#wasm2wat --enable-threads ${outbase}_link.wasm -o ${outbase}_link.wat

clang --target=wasm32-wasi-threads -O1 -pthread --sysroot=$sysroot_dir $cfile -c -o $outbase.wasm $verbose
wasm2wat --enable-threads $outbase.wasm -o $outbase.wat
wasm-ld --no-entry --shared-memory --export-memory --import-memory --max-memory=67108864 --allow-undefined -L$sysroot_dir/lib $outbase.wasm $crtfile -lc -lm $rt_lib -o ${outbase}_link.wasm
wasm2wat --enable-threads ${outbase}_link.wasm -o ${outbase}_link.wat
