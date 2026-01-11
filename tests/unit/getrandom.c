// CMD: args="random"

#include "wali_start.c"
#include <unistd.h>
#include <stddef.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getrandom")))
long __imported_wali_getrandom(void *buf, size_t buflen, unsigned int flags);

int wali_getrandom(void *buf, size_t buflen, unsigned int flags) {
  return (int) __imported_wali_getrandom(buf, buflen, flags);
}
#else
#include <sys/syscall.h>
#ifndef SYS_getrandom
#define SYS_getrandom 318
#endif
int wali_getrandom(void *buf, size_t buflen, unsigned int flags) {
  return syscall(SYS_getrandom, buf, buflen, flags);
}
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  
  char buf[32];
  for(int i=0; i<32; i++) buf[i] = 0;
  
  // flags=0
  int res = wali_getrandom(buf, 32, 0);
  if (res != 32) return -1;
  
  // Check if buffer changed (probability of all 0s is low)
  int all_zeros = 1;
  for(int i=0; i<32; i++) {
      if (buf[i] != 0) all_zeros = 0;
  }
  if (all_zeros) return -1;
  
  return 0;
}
