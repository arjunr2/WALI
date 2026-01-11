// CMD: setup="create /tmp/read_ok.txt" args="file /tmp/read_ok.txt"
// CMD: args="bad_fd"

#include "wali_start.c"
#include <fcntl.h>
#include <stddef.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *mode = argv[0];
    const char *path = argv[1];
    
    if (strcmp(mode, "create") == 0) {
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) return 1;
        if (write(fd, "WALI_READ_TEST", 14) != 14) {
            close(fd);
            return 1;
        }
        close(fd);
    }
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
     const char *mode = argv[0];
    const char *path = argv[1];
    if (strcmp(mode, "create") == 0) {
        unlink(path);
    }
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_read")))
long __imported_wali_read(int fd, void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_open(const char *pathname, int flags, int mode) {
  return (int) __imported_wali_open(pathname, flags, mode);
}
int wali_read(int fd, void *buf, size_t count) {
  return (int) __imported_wali_read(fd, buf, count);
}
int wali_close(int fd) {
  return (int) __imported_wali_close(fd);
}
#else
#include <unistd.h>
#include <sys/syscall.h>
int wali_open(const char *pathname, int flags, int mode) {
  return syscall(SYS_open, pathname, flags, mode);
}
int wali_read(int fd, void *buf, size_t count) {
  return syscall(SYS_read, fd, buf, count);
}
int wali_close(int fd) {
  return syscall(SYS_close, fd);
}
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *mode = (argc > 0) ? argv[0] : "bad_fd";
  
  if (strcmp(mode, "file") == 0) {
      if (argc < 2) return -1;
      int fd = wali_open(argv[1], O_RDONLY, 0);
      if (fd < 0) return -1;
      
      char buf[32];
      memset(buf, 0, sizeof(buf));
      int bytes = wali_read(fd, buf, 32);
      wali_close(fd);
      
      if (bytes != 14) return -1;
      
      // Check content logic
      // We can copy to result_buffer to be extra safe if we wanted the runner to check
      // But user said "if exit code is not enough". Here compare is easy inside C.
      if (strcmp(buf, "WALI_READ_TEST") != 0) return -1;
      
      // Store in result buffer just in case
      strcpy((char*)result_buffer, buf);
      
      return 0;
  } else if (strcmp(mode, "bad_fd") == 0) {
      char buf[10];
      int res = wali_read(9999, buf, 10);
      if (res >= 0) return -1; // Should fail
      return 0;
  }
  
  return -1;
}
