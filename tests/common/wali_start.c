#ifndef WALI_START_H_
#define WALI_START_H_

#include "wali_test_helpers.h"

// Declaration of the test function implemented by each unit test
void test(void);

// If building the test wrapper, we skip the main definition
// The wrapper provides its own main and calls setups/cleanup
#ifndef WALI_TEST_WRAPPER

/* Definition of the main/start function */
#ifdef __wasm__
// We export "_start" for the Wasm entrypoint
__attribute__((export_name("_start")))
void _start(void) {
#else
int main(int argc, char **argv) {
#endif
  int r = 0;
  r = wali_init();
  if (r) {
    goto fail;
  }
  test();
  int s = wali_deinit();
  if (s) {
    r = s;
  }
fail:
  wali_proc_exit(r);
#ifndef __wasm__
  return 0;
#endif
}

#endif // WALI_TEST_WRAPPER


#endif /* WALI_START_H_ */