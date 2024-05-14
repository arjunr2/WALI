#!/bin/bash

source ../wali_config.sh

outdir=.

rt_lib=$WALI_LIBCLANG_RT_LIB

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

crtfile=$WALI_SYSROOT_DIR/lib/crt1.o 

$WALI_CC $verbose $WALI_CFLAGS $cfile -c -o $outbase.int.wasm
wasm2wat --enable-threads $outbase.int.wasm -o $outbase.int.wat
$WALI_LD $verbose --no-gc-sections --no-entry --shared-memory --export-memory --max-memory=67108864 --allow-undefined -L$sysroot_dir/lib $outbase.int.wasm $crtfile -lc -lm $rt_lib -o ${outbase}.wasm
wasm2wat --enable-threads ${outbase}.wasm -o ${outbase}.wat
