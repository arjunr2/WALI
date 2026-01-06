#ifndef WALI_TEST_HELPERS_H
#define WALI_TEST_HELPERS_H

#include <stdint.h>

// Buffer for test setup - 64KB aligned to 64KB
static uint8_t result_buffer[65536] __attribute__((aligned(65536)));

#ifdef __wasm__

// Imports
__attribute__((__import_module__("env"), __import_name__("wali_test_setup")))
int __imported_wali_test_setup(uint32_t offset, uint32_t len);

__attribute__((__import_module__("wali"), __import_name__("__init")))
int __imported_wali_init(void);

__attribute__((__import_module__("wali"), __import_name__("__deinit")))
int __imported_wali_deinit(void);

__attribute__((__import_module__("wali"), __import_name__("__proc_exit")))
void __imported_wali_proc_exit(int code);

static int wali_test_setup(void *ptr, uint32_t len) {
  // In Wasm, pointers are 32-bit offsets
  return __imported_wali_test_setup(ptr, len);
}

static int wali_init(void) {
  if (wali_test_setup(result_buffer, sizeof(result_buffer)) != 0) {
    return -1;
  }
  return __imported_wali_init();
}

static int wali_deinit() {
  return __imported_wali_deinit();
}

static void wali_proc_exit(int code) {
    __imported_wali_proc_exit(code);
}

#else // Native implementation

#include <stdio.h>

static int wali_test_setup(void *ptr, uint32_t len) {
  // Native implementation of the check
  printf("[Native] Checking buffer at %p with len: %u\n", ptr, len);
  // We can add alignment checks here if we want to match the Wasm behavior strictly,
  // but native pointers might not be aligned to 64k.
  return 0;
}

static int wali_init(void) {
  if (wali_test_setup(result_buffer, sizeof(result_buffer)) != 0) {
    return -1;
  }
  return 0;
}

static int wali_deinit() {
  return 0;
}

static void wali_proc_exit(int code) {
  return;
}

#endif

#endif
