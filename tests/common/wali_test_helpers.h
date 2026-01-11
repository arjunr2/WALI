#ifndef WALI_TEST_HELPERS_H
#define WALI_TEST_HELPERS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>

// Buffer for test setup - 64KB aligned to 64KB
static uint8_t result_buffer[65536] __attribute__((aligned(65536)));

// Global variables to hold CLI args
extern int argc;
extern char **argv;

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

/* Command Line Argument handling for WALI */
__attribute__((__import_module__("wali"), __import_name__("__cl_get_argc")))
int __imported_wali_cl_get_argc(void);

__attribute__((__import_module__("wali"), __import_name__("__cl_get_argv_len")))
int __imported_wali_cl_get_argv_len(int);

__attribute__((__import_module__("wali"), __import_name__("__cl_copy_argv")))
int __imported_wali_cl_copy_argv(char*, int);

static int wali_test_setup(void *ptr, uint32_t len) {
  // In Wasm, pointers are 32-bit offsets
  return __imported_wali_test_setup(ptr, len);
}

static int wali_init(void) {
  if (wali_test_setup(result_buffer, sizeof(result_buffer)) != 0) {
    return -1;
  }
  // Initialize to non-zero to detect missing writes
  memset(result_buffer, 0xCC, sizeof(result_buffer));
  return __imported_wali_init();
}

static int wali_deinit() {
  return __imported_wali_deinit();
}

static void wali_proc_exit(int code) {
    __imported_wali_proc_exit(code);
}

char wasm_args[256][1024];
char *wasm_argv_ptrs[256];

extern int putenv(char *string);

/* Environment Variable handling */
__attribute__((__import_module__("wali"), __import_name__("__get_init_envfile")))
int __imported_wali_get_init_envfile(char* buf, int buf_len);

static int test_init_env(void) {
  char line[1024];
  int found = __imported_wali_get_init_envfile(line, sizeof(line)) != 0;
  if (!found) {
     return 0; // Not an error if just not set
  }

  FILE *f = fopen(line, "r");
  if (!f) return 1;

  while (fgets(line, sizeof(line), f)) {
      size_t len = strlen(line);
      if (len >= 1023) {
          fclose(f);
          return 1; // Line too long
      }
      if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
      putenv(line);
  }
  fclose(f);
  return 0;
}

static int test_init_args(void) {
  int c = __imported_wali_cl_get_argc();
  if (c > 256) {
    return 1;
  }
  for (int i = 0; i < c; i++) {
    int arg_len = __imported_wali_cl_get_argv_len(i);
    if (arg_len >= 1024) {
      return 1;
    }
    __imported_wali_cl_copy_argv(wasm_args[i], i);
    wasm_argv_ptrs[i] = wasm_args[i];
  }
  argc = c;
  argv = wasm_argv_ptrs;
  return 0;
}

#else // Native implementation

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static int wali_test_setup(void *ptr, uint32_t len) {
  const char *filepath = getenv("WALI_TEST_RESULT_FILE");
  if (!filepath) {
      if (getenv("WALI_TEST_VERBOSE")) printf("[Native] WALI_TEST_RESULT_FILE not set. Using in-memory buffer.\n");
      return 0; // Fallback to normal memory
  }

  int fd = open(filepath, O_RDWR | O_CREAT, 0666);
  if (fd < 0) {
      perror("[Native] open");
      return -1;
  }
  if (ftruncate(fd, len) != 0) {
      perror("[Native] ftruncate");
      close(fd);
      return -1;
  }
  
  void *res = mmap(ptr, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
  close(fd);
  
  if (res == MAP_FAILED) {
      perror("[Native] mmap");
      return -1;
  }

  return 0;
}

static int wali_init(void) {
  if (wali_test_setup(result_buffer, sizeof(result_buffer)) != 0) {
    return -1;
  }
  // Initialize to non-zero to detect missing writes
  memset(result_buffer, 0xCC, sizeof(result_buffer));
  return 0;
}


static int wali_deinit() {
  return 0;
}

static void wali_proc_exit(int code) {
  exit(code);
}

static int test_init_env(void) {
  return 0;
}

static int test_init_args(void) {
  // Native already has argc/argv sets
  return 0;
}

#endif

#endif
