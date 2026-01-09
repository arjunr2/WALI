#include "wali_start.c"
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WALI_TEST_WRAPPER
// Setup/Cleanup hooks
// RUN: wali_test_file.txt
// RUN: other_file.txt

int test_setup(int argc, char **argv) {
    const char *fname = "wali_test_file.txt";
    if (argc >= 1) {
        fname = argv[0];
    }
    printf("[Setup] Creating file: %s\n", fname);

    int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("[Setup] Failed to create file");
        return 1;
    }
    if (write(fd, "Hello WALI", 10) == -1) {
        perror("[Setup] Failed to write to file");
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

int test_cleanup(int argc, char **argv) {
    const char *fname = "wali_test_file.txt";
    if (argc >= 1) {
        fname = argv[0];
    }
    
    if (unlink(fname) == -1) {
        perror("[Cleanup] Failed to remove file");
        return 1;
    }
    return 0;
}
#endif

#ifdef __wasm__
// Import WALI open syscall
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);

int wali_open(const char *pathname, int flags, int mode) {
  return (int) __imported_wali_open(pathname, flags, mode);
}
#else
// Native: use raw syscall
#include <unistd.h>
#include <sys/syscall.h>

int wali_open(const char *pathname, int flags, int mode) {
  return syscall(SYS_open, pathname, flags, mode);
}
#endif

void test(void) {
  // Test Case: Open wali_test_file.txt (created by setup)
  const char *path = "wali_test_file.txt";
  int fd = wali_open(path, O_RDONLY, 0);

  // Store result in result_buffer
  // Format: [int32: fd] 
  // Note: We can't guarantee exact fd number, but it should be valid (>=0)
  // Let's store 0 if success, -1 if fail
  int status = (fd >= 0) ? 0 : -1;
  *((int *)result_buffer) = status;
}
