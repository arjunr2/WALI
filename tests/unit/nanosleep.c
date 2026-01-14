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
__attribute__((__import_module__("wali"), __import_name__("SYS_nanosleep")))
long __imported_wali_nanosleep(const struct timespec *req, struct timespec *rem);
__attribute__((__import_module__("wali"), __import_name__("SYS_clock_gettime")))
long __imported_wali_clock_gettime(clockid_t clockid, struct timespec *tp);

int wali_nanosleep(const struct timespec *req, struct timespec *rem) { return (int)__imported_wali_nanosleep(req, rem); }
int wali_clock_gettime(clockid_t clockid, struct timespec *tp) { return (int)__imported_wali_clock_gettime(clockid, tp); }

#else
#include <sys/syscall.h>
int wali_nanosleep(const struct timespec *req, struct timespec *rem) { return syscall(SYS_nanosleep, req, rem); }
int wali_clock_gettime(clockid_t clockid, struct timespec *tp) { return syscall(SYS_clock_gettime, clockid, tp); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  
  // To verify nanosleep, we can use clock_gettime if we trust it, or just rely on exit code.
  // The original test verified time elapsed. That's good practice.
  
  struct timespec ts_start = {0};
  if (wali_clock_gettime(CLOCK_MONOTONIC, &ts_start) != 0) return -1;
  
  struct timespec req = {0};
  req.tv_sec = 0;
  req.tv_nsec = 50 * 1000 * 1000; // 50ms
  
  if (wali_nanosleep(&req, NULL) != 0) return -1;
  
  struct timespec ts_end = {0};
  if (wali_clock_gettime(CLOCK_MONOTONIC, &ts_end) != 0) return -1;
  
  long diff_ms = (ts_end.tv_sec - ts_start.tv_sec) * 1000 + (ts_end.tv_nsec - ts_start.tv_nsec) / 1000000;
  
  if (diff_ms < 40) return -1; 
  
  return 0;
}
