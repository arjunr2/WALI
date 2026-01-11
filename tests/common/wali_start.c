#ifndef WALI_START_H_
#define WALI_START_H_

#include "wali_test_helpers.h"

// Global args storage
int argc = 0;
char **argv = 0;

// Declaration of the test function implemented by each unit test
int test(void);

// If building the test wrapper, we skip the main definition
// The wrapper provides its own main and calls setups/cleanup
#ifndef WALI_TEST_WRAPPER

/* Definition of the main/start function */
#ifdef __wasm__
// We export "_start" for the Wasm entrypoint
__attribute__((export_name("_start")))
void _start(void) {
  // TODO: Fetch args from WALI environment if possible
#else
int main(int _argc, char **_argv) {
  argc = _argc;
  argv = _argv;
#endif
  int r = 0;
  r = wali_init();
  if (r) {
    goto fail;
  }
  int test_ret = test();
  r = test_ret;
  
  if (r == 0) {
    int s = wali_deinit();
    if (s) {
      r = s;
    }
  }
fail:
  wali_proc_exit(r);
#ifndef __wasm__
  return r;
#endif
}

#endif // WALI_TEST_WRAPPER


#endif /* WALI_START_H_ */