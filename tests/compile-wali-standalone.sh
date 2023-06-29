#!/bin/bash

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

# Compile standalone C file
clang \
  --target=wasm32-wasi-threads -O3 -pthread \
  `# Sysroot and lib search path` \
  --sysroot=$sysroot_dir -L$sysroot_dir/lib \
  `# Enable wasm extension features`  \
  -matomics -mbulk-memory -mmutable-globals -msign-ext  \
  `# Linker flags for shared mem + threading` \
  -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=67108864 \
  $cfile -o $outbase.wasm $verbose

wasm2wat --enable-threads $outbase.wasm -o $outbase.wat
