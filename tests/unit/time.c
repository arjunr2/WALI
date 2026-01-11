// CMD: args="time"

#include "wali_start.c"
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

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
__attribute__((__import_module__("wali"), __import_name__("SYS_gettimeofday")))
long __imported_wali_gettimeofday(struct timeval *tv, struct timezone *tz);
__attribute__((__import_module__("wali"), __import_name__("SYS_clock_gettime")))
long __imported_wali_clock_gettime(clockid_t clockid, struct timespec *tp);
__attribute__((__import_module__("wali"), __import_name__("SYS_nanosleep")))
long __imported_wali_nanosleep(const struct timespec *req, struct timespec *rem);

int wali_gettimeofday(struct timeval *tv, struct timezone *tz) { return (int)__imported_wali_gettimeofday(tv, tz); }
int wali_clock_gettime(clockid_t clockid, struct timespec *tp) { return (int)__imported_wali_clock_gettime(clockid, tp); }
int wali_nanosleep(const struct timespec *req, struct timespec *rem) { return (int)__imported_wali_nanosleep(req, rem); }

#else
#include <sys/syscall.h>
int wali_gettimeofday(struct timeval *tv, struct timezone *tz) { return syscall(SYS_gettimeofday, tv, tz); }
int wali_clock_gettime(clockid_t clockid, struct timespec *tp) { return syscall(SYS_clock_gettime, clockid, tp); }
int wali_nanosleep(const struct timespec *req, struct timespec *rem) { return syscall(SYS_nanosleep, req, rem); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  
  // 1. gettimeofday
  struct timeval tv = {0};
  if (wali_gettimeofday(&tv, NULL) != 0) return -1;
  if (tv.tv_sec == 0) return -1; // Unless perfectly at epoch (unlikely)
  
  // 2. clock_gettime
  struct timespec ts_start = {0};
  if (wali_clock_gettime(CLOCK_MONOTONIC, &ts_start) != 0) return -1;
  
  // 3. nanosleep 50ms
  struct timespec req = {0};
  req.tv_sec = 0;
  req.tv_nsec = 50 * 1000 * 1000;
  
  if (wali_nanosleep(&req, NULL) != 0) return -1;
  
  // 4. clock_gettime check
  struct timespec ts_end = {0};
  if (wali_clock_gettime(CLOCK_MONOTONIC, &ts_end) != 0) return -1;
  
  long diff_ms = (ts_end.tv_sec - ts_start.tv_sec) * 1000 + (ts_end.tv_nsec - ts_start.tv_nsec) / 1000000;
  
  if (diff_ms < 40) return -1; // Allow some slack, but should be at least close to 50
  
  return 0;
}
