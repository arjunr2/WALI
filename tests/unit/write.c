#include "wali_start.c"
#include <fcntl.h>
#include <stddef.h>

#ifdef __wasm__
// Import WALI syscalls
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);

__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);

__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_open(const char *pathname, int flags, int mode) {
  return (int) __imported_wali_open(pathname, flags, mode);
}
int wali_write(int fd, const void *buf, size_t count) {
  return (int) __imported_wali_write(fd, buf, count);
}
int wali_close(int fd) {
  return (int) __imported_wali_close(fd);
}
#else
// Native: use raw syscall
#include <unistd.h>
#include <sys/syscall.h>

int wali_open(const char *pathname, int flags, int mode) {
  return syscall(SYS_open, pathname, flags, mode);
}
int wali_write(int fd, const void *buf, size_t count) {
  return syscall(SYS_write, fd, buf, count);
}
int wali_close(int fd) {
  return syscall(SYS_close, fd);
}
#endif

int test(void) {
  // Test Case: Write to /dev/null
  // Expected: success, written bytes == requested bytes
  
  const char *path = "/dev/null";
  int fd = wali_open(path, O_WRONLY, 0);
  
  // Format: [int32: open_status] [int32: written_bytes]
  int status = (fd >= 0) ? 0 : -1;
  *((int *)result_buffer) = status;
  
  if (fd < 0) return 0;

  const char *msg = "Hello WALI";
  size_t len = 10;
  int written = wali_write(fd, msg, len);
  
  wali_close(fd);

  *(((int *)result_buffer) + 1) = written;
  return 0;
}
