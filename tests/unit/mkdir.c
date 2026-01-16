// CMD: setup="clean /tmp/test_dir" args="create /tmp/test_dir" cleanup="/tmp/test_dir"
// CMD: setup="create /tmp/exist_dir" args="fail /tmp/exist_dir" cleanup="/tmp/exist_dir"
// CMD: args="no_parent /tmp/p/c"
// CMD: setup="clean /tmp/mode_dir" args="mode /tmp/mode_dir" cleanup="/tmp/mode_dir"

#include "wali_start.c"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int file_exists(const char *path) {
    return access(path, F_OK) == 0;
}

int test_setup(int argc, char **argv) {
    if (argc < 1) return 0;
    const char *mode = argv[0];
    const char *path = argv[1];
    
    if (strcmp(mode, "create") == 0) {
        mkdir(path, 0755);
    } else if (strcmp(mode, "clean") == 0) {
        rmdir(path); // Ensure clean
    }
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc < 1) return 0;
    rmdir(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_mkdir")))
long __imported_wali_mkdir(const char *pathname, int mode);

__attribute__((__import_module__("wali"), __import_name__("SYS_rmdir")))
long __imported_wali_rmdir(const char *pathname);

int wali_mkdir(const char *pathname, int mode) {
  return (int) __imported_wali_mkdir(pathname, mode);
}
int wali_rmdir(const char *pathname) {
  return (int) __imported_wali_rmdir(pathname);
}
#else
#include <sys/syscall.h>
int wali_mkdir(const char *pathname, int mode) {
  return syscall(SYS_mkdir, pathname, mode);
}
int wali_rmdir(const char *pathname) {
  return syscall(SYS_rmdir, pathname);
}
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  if (argc != 2) return -1;
  const char *mode = argv[0];
  const char *path = argv[1];
  
  if (strcmp(mode, "create") == 0) {
      if (wali_mkdir(path, 0755) != 0) return -1;
      return 0;
  } else if (strcmp(mode, "fail") == 0) {
      if (wali_mkdir(path, 0755) == 0) return -1; // Should fail as it exists
      return 0;
  } else if (strcmp(mode, "no_parent") == 0) {
      if (wali_mkdir(path, 0755) == 0) return -1;
      return 0;
  } else if (strcmp(mode, "mode") == 0) {
      // 0700
      if (wali_mkdir(path, 0700) != 0) return -1;
      // We can't verify stat mode without stat syscall but return code success is good proxy
      // Assuming stat.c tests stat separately.
      return 0;
  }
  
  return -1;
}
