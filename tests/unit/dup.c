// CMD: setup="create /tmp/dup_test.txt" args="file /tmp/dup_test.txt"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
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
        if (write(fd, "DUP_TEST", 8) != 8) {
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
__attribute__((__import_module__("wali"), __import_name__("SYS_dup")))
long __imported_wali_dup(int fd);

int wali_open(const char *pathname, int flags, int mode) {
  return (int) __imported_wali_open(pathname, flags, mode);
}
int wali_read(int fd, void *buf, size_t count) {
  return (int) __imported_wali_read(fd, buf, count);
}
int wali_close(int fd) {
  return (int) __imported_wali_close(fd);
}
int wali_dup(int fd) {
  return (int) __imported_wali_dup(fd);
}
#else
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
int wali_dup(int fd) {
  return syscall(SYS_dup, fd);
}
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  if (argc < 2) return -1;
  const char *path = argv[1];
  
  int fd = wali_open(path, O_RDONLY, 0);
  if (fd < 0) return -1;
  
  // Test dup
  int fd2 = wali_dup(fd);
  if (fd2 < 0) { wali_close(fd); return -1; }
  if (fd2 == fd) { wali_close(fd); wali_close(fd2); return -1; }
  
  char buf[16];
  memset(buf, 0, sizeof(buf));
  if (wali_read(fd2, buf, 8) != 8) {
       wali_close(fd); wali_close(fd2); return -1; 
  }
  if (strcmp(buf, "DUP_TEST") != 0) {
      wali_close(fd); wali_close(fd2); return -1;
  }
  wali_close(fd2);
  
  wali_close(fd);
  
  return 0;
}
