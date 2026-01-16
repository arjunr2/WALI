// CMD: setup="/tmp/unlink_ok.txt" args="clean /tmp/unlink_ok.txt" cleanup="/tmp/unlink_ok.txt"
// CMD: args="fail /tmp/unlink_fail.txt"

#include "wali_start.c"
#include <fcntl.h>
#include <stddef.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int file_exists(const char *path) {
    return access(path, F_OK) == 0;
}

int test_setup(int argc, char **argv) {
    if (argc < 1) return 0;
    const char *path = argv[0];
    
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    close(fd);
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc < 1) return 0;
    const char *path = argv[0];
    if (file_exists(path)) {
        unlink(path);
    }
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_unlink")))
long __imported_wali_unlink(const char *pathname);

int wali_unlink(const char *pathname) {
  return (int) __imported_wali_unlink(pathname);
}
#else
#include <unistd.h>
#include <sys/syscall.h>
int wali_unlink(const char *pathname) {
  return syscall(SYS_unlink, pathname);
}
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  if (argc < 2) return -1;
  const char *mode = argv[0];
  const char *path = argv[1];
  
  if (strcmp(mode, "clean") == 0) {
      if (wali_unlink(path) != 0) return -1;
      // We cannot easily check if file exists from WASM without access/stat syscalls 
      // which we might not have linked or tested yet.
      // But we can check return code.
      // And wrapper verification ensures side-effect.
      return 0;
  } else if (strcmp(mode, "fail") == 0) {
      if (wali_unlink(path) == 0) return -1; // Should fail
      return 0;
  }
  
  return -1;
}
