#!/bin/bash

SYSROOT_DIR=/home/arjun/Documents/research/webassembly/wali/wali-musl/sysroot
cflag_warning_ign="-Wno-implicit-function-declaration -Wno-int-conversion -Wno-incompatible-function-pointer-types"
wali_cflags="-O0 --target=wasm32-wasi-threads -pthread $cflag_warning_ign --sysroot=$SYSROOT_DIR -L$SYSROOT_DIR/lib -matomics -mbulk-memory -mmutable-globals -msign-ext"
wali_lflags="-Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=67108864 -lpthread"

./configure --without-openssl --with-zlib="/home/arjun/Documents/research/webassembly/wali/libsrc/zlib" \
  CC=clang  \
  CC_FOR_BUILD=clang \
  CFLAGS="$wali_cflags" \
  LD=wasm-ld \
  LDFLAGS="$wali_cflags $wali_lflags" \
  LIBS="-lpthread" \
  AR=llvm-ar RANLIB=llvm-ranlib \
  cross_compiling=yes \

