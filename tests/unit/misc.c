// CMD: args="uname"

#include "wali_start.c"
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    return 0;
}

int test_cleanup(int argc, char **argv) {
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_uname")))
long long __imported_wali_uname(struct utsname *buf);

int wali_uname(struct utsname *buf) { return (int)__imported_wali_uname(buf); }

#else
#include <sys/syscall.h>
int wali_uname(struct utsname *buf) { return syscall(SYS_uname, buf); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *mode = (argc > 0) ? argv[0] : "fail";
  
  if (strcmp(mode, "uname") == 0) {
      struct utsname u;
      if (wali_uname(&u) != 0) return -1;
      
      // Basic sanity check
      if (strlen(u.sysname) == 0) return -1;
      
      // WALI usually reports Linux or WALI?
      // Just check it's not empty.
      
      return 0;
  }
  
  return -1;
}
