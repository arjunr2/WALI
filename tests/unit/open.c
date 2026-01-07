#include "wali_start.c"
#include <fcntl.h>

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
  // Test Case: Open /dev/null
  // Expected: success (fd >= 0)
  
  const char *path = "/dev/null";
  int fd = wali_open(path, O_RDONLY, 0);
  
  // Store result in result_buffer
  // Format: [int32: status] (0 for success, -1 for fail)
  int status = (fd >= 0) ? -1 : 0;
  *((int *)result_buffer) = status;
}
