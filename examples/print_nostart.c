// Compile with `-nostartfiles` and `-Wl,--no-entry` to stub out main/start files
//
// Usually crt startup automatically calls the WALI __init and __deinit startups for musl-libc.
// You can export `__wali_startup` and `__wali_cleanup` within musl using the 
// wasm-ld linker flag `--export` to call them directly from an embedding without crt

#include "stdio.h"

// We can call this method after `__wali_startup` to run
// some WALI code that uses musl-libc.
__attribute__((export_name("print")))
int print() {
  return write(1, "Hello world: 42\n", 16);
}
