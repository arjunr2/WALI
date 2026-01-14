// CMD: args="basic"

#include "wali_start.c"
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_gettimeofday")))
long __imported_wali_gettimeofday(struct timeval *tv, struct timezone *tz);

int wali_gettimeofday(struct timeval *tv, struct timezone *tz) { return (int)__imported_wali_gettimeofday(tv, tz); }

#else
#include <sys/syscall.h>
int wali_gettimeofday(struct timeval *tv, struct timezone *tz) { return syscall(SYS_gettimeofday, tv, tz); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  
  struct timeval tv = {0};
  if (wali_gettimeofday(&tv, NULL) != 0) return -1;
  if (tv.tv_sec == 0) return -1; // Unless perfectly at epoch
  
  return 0;
}
