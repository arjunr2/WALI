#include "common.h"
#include <errno.h>

#ifdef __wasm__
long wali_SYS_getuid(void) __attribute ((
  __import_module__("wali"),
  __import_name__("SYS_getuid")
));

long wali_SYS_getpid(void) __attribute ((
  __import_module__("wali"),
  __import_name__("SYS_getpid")
));
#else
// Native target just uses native syscalls
long wali_SYS_getuid(void) { return getuid(); }
long wali_SYS_getpid(void) { return getpid(); }
#endif

int main() {
  long (*wali_funcptrs[]) (void) = {
    wali_SYS_getuid,
    wali_SYS_getpid,
  };

  printf("Hello from UID: %ld in PID: %ld\n", 
      wali_funcptrs[0](), wali_funcptrs[1]());
  return 0;
}
