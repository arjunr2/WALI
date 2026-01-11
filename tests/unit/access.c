// CMD: setup="create /tmp/access_ok.txt" args="exist /tmp/access_ok.txt"
// CMD: args="fail /tmp/access_missing.txt"
// CMD: setup="create /tmp/access_rw.txt" args="read /tmp/access_rw.txt"
// CMD: setup="create /tmp/access_rw.txt" args="write /tmp/access_rw.txt"
// CMD: setup="create_exec /tmp/access_x.txt" args="exec /tmp/access_x.txt"
// CMD: setup="create_ro /tmp/access_ro.txt" args="no_write /tmp/access_ro.txt"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *mode = argv[0];
    const char *path = argv[1];
    
    if (strcmp(mode, "create") == 0) {
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) return 1;
        close(fd);
    } else if (strcmp(mode, "create_exec") == 0) {
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
        if (fd < 0) return 1;
        close(fd);
    } else if (strcmp(mode, "create_ro") == 0) { // Read only
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0444);
        if (fd < 0) return 1;
        close(fd);
    }
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *mode = argv[0];
    const char *path = argv[1];
    if (strchr(mode, '_') != NULL || strcmp(mode, "create") == 0) {
        unlink(path);
    }
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_access")))
long __imported_wali_access(const char *pathname, int mode);

int wali_access(const char *pathname, int mode) {
  return (int) __imported_wali_access(pathname, mode);
}
#else
#include <sys/syscall.h>
int wali_access(const char *pathname, int mode) {
  return syscall(SYS_access, pathname, mode);
}
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *mode = (argc > 0) ? argv[0] : "fail";
  const char *path = (argc > 1) ? argv[1] : "";

  if (strcmp(mode, "exist") == 0) {
      if (wali_access(path, F_OK) != 0) return -1;
      if (wali_access(path, R_OK) != 0) return -1;
      return 0;
  } else if (strcmp(mode, "fail") == 0) {
      if (wali_access(path, F_OK) == 0) return -1;
      return 0;
  } else if (strcmp(mode, "read") == 0) {
      if (wali_access(path, R_OK) != 0) return -1;
      return 0;
  } else if (strcmp(mode, "write") == 0) {
      if (wali_access(path, W_OK) != 0) return -1;
      return 0;
  } else if (strcmp(mode, "exec") == 0) {
      if (wali_access(path, X_OK) != 0) return -1;
      return 0;
  } else if (strcmp(mode, "no_write") == 0) {
      // Should fail W_OK
      if (wali_access(path, W_OK) == 0) return -1; 
      // But R_OK should pass
      if (wali_access(path, R_OK) != 0) return -1;
      return 0;
  }
  
  return -1;
}
