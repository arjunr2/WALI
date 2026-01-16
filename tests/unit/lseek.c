// CMD: setup="/tmp/seek_test.txt" args="file /tmp/seek_test.txt" cleanup="/tmp/seek_test.txt"
// CMD: args="bad_fd"
// CMD: setup="/tmp/seek_past.txt" args="past /tmp/seek_past.txt" cleanup="/tmp/seek_past.txt"

#include "wali_start.c"
#include <fcntl.h>
#include <stddef.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    if (argc < 1) return 0;
    const char *path = argv[0];
    
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    // writes "0123456789"
    if (write(fd, "0123456789", 10) != 10) {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc < 1) return 0;
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_read")))
long __imported_wali_read(int fd, void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_lseek")))
long __imported_wali_lseek(int fd, long offset, int whence);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_open(const char *pathname, int flags, int mode) {
  return (int) __imported_wali_open(pathname, flags, mode);
}
int wali_read(int fd, void *buf, size_t count) {
  return (int) __imported_wali_read(fd, buf, count);
}
long wali_lseek(int fd, long offset, int whence) {
  return __imported_wali_lseek(fd, offset, whence);
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
long wali_lseek(int fd, long offset, int whence) {
  return syscall(SYS_lseek, fd, offset, whence);
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
      
      char ch;
      // 1. SEEK_SET to 5. Expect '5'
      long res = wali_lseek(fd, 5, SEEK_SET);
      if (res != 5) { wali_close(fd); return -1; }
      
      if (wali_read(fd, &ch, 1) != 1) { wali_close(fd); return -1; }
      if (ch != '5') { wali_close(fd); return -1; }
      
      // 2. SEEK_CUR -2 (from 6 to 4). Expect '4'
      res = wali_lseek(fd, -2, SEEK_CUR);
      if (res != 4) { wali_close(fd); return -1; }
      
      if (wali_read(fd, &ch, 1) != 1) { wali_close(fd); return -1; }
      if (ch != '4') { wali_close(fd); return -1; }
      
      // 3. SEEK_END -1. Expect '9'. File size is 10. End is 10. 10-1 = 9.
      res = wali_lseek(fd, -1, SEEK_END);
      if (res != 9) { wali_close(fd); return -1; }
      
      if (wali_read(fd, &ch, 1) != 1) { wali_close(fd); return -1; }
      if (ch != '9') { wali_close(fd); return -1; }
      
      wali_close(fd);
      return 0;
  } else if (strcmp(mode, "bad_fd") == 0) {
      long res = wali_lseek(9999, 0, SEEK_SET);
      if (res >= 0) return -1;
      return 0;
  } else if (strcmp(mode, "past") == 0) {
      if (argc < 2) return -1;
      int fd = wali_open(argv[1], O_RDONLY, 0); 
      if (fd < 0) return -1;
      // Seek past end (10) -> 15. Allowed for read (eof)
      long res = wali_lseek(fd, 15, SEEK_SET);
      if (res != 15) return -1;
      
      char ch;
      // Read should return 0
      if (wali_read(fd, &ch, 1) != 0) return -1;
      
      wali_close(fd);
      return 0;
  }
  
  return -1;
}
