// CMD: setup="create /tmp/io_file" args="writev /tmp/io_file"
// CMD: setup="create /tmp/io_file" args="readv /tmp/io_file"
// CMD: setup="create /tmp/io_file" args="pwrite /tmp/io_file"
// CMD: setup="create /tmp/io_file" args="pread /tmp/io_file"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>

int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *mode = argv[0];
    const char *fname = argv[1];
    
    if (strcmp(mode, "create") == 0) {
        int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd >= 0) {
            write(fd, "0123456789", 10);
            close(fd);
        }
    }
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_readv")))
long long __imported_wali_readv(int fd, const struct iovec *iov, int iovcnt);

__attribute__((__import_module__("wali"), __import_name__("SYS_writev")))
long long __imported_wali_writev(int fd, const struct iovec *iov, int iovcnt);

__attribute__((__import_module__("wali"), __import_name__("SYS_pread64")))
long long __imported_wali_pread64(int fd, void *buf, size_t count, long long offset);

__attribute__((__import_module__("wali"), __import_name__("SYS_pwrite64")))
long long __imported_wali_pwrite64(int fd, const void *buf, size_t count, long long offset);

ssize_t wali_readv(int fd, const struct iovec *iov, int iovcnt) { return (ssize_t)__imported_wali_readv(fd, iov, iovcnt); }
ssize_t wali_writev(int fd, const struct iovec *iov, int iovcnt) { return (ssize_t)__imported_wali_writev(fd, iov, iovcnt); }
ssize_t wali_pread(int fd, void *buf, size_t count, off_t offset) { return (ssize_t)__imported_wali_pread64(fd, buf, count, (long long)offset); }
ssize_t wali_pwrite(int fd, const void *buf, size_t count, off_t offset) { return (ssize_t)__imported_wali_pwrite64(fd, buf, count, (long long)offset); }

#else
#include <sys/syscall.h>
ssize_t wali_readv(int fd, const struct iovec *iov, int iovcnt) { return syscall(SYS_readv, fd, iov, iovcnt); }
ssize_t wali_writev(int fd, const struct iovec *iov, int iovcnt) { return syscall(SYS_writev, fd, iov, iovcnt); }
ssize_t wali_pread(int fd, void *buf, size_t count, off_t offset) { return syscall(SYS_pread64, fd, buf, count, offset); }
ssize_t wali_pwrite(int fd, const void *buf, size_t count, off_t offset) { return syscall(SYS_pwrite64, fd, buf, count, offset); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *op = argv[0];
  const char *fname = argv[1];
  
  if (strcmp(op, "readv") == 0) {
      int fd = open(fname, O_RDONLY);
      if (fd < 0) return -1;
      
      char buf1[3]; // "012"
      char buf2[3]; // "345"
      struct iovec iov[2];
      iov[0].iov_base = buf1;
      iov[0].iov_len = 3;
      iov[1].iov_base = buf2;
      iov[1].iov_len = 3;
      
      ssize_t n = wali_readv(fd, iov, 2);
      close(fd);
      
      if (n != 6) return -1;
      if (strncmp(buf1, "012", 3) != 0) return -1;
      if (strncmp(buf2, "345", 3) != 0) return -1;
      return 0;
      
  } else if (strcmp(op, "writev") == 0) {
      int fd = open(fname, O_WRONLY | O_TRUNC);
      if (fd < 0) return -1;
      
      char *str1 = "AB";
      char *str2 = "CD";
      struct iovec iov[2];
      iov[0].iov_base = str1;
      iov[0].iov_len = 2;
      iov[1].iov_base = str2;
      iov[1].iov_len = 2;
      
      ssize_t n = wali_writev(fd, iov, 2);
      close(fd);
      
      if (n != 4) return -1;
      
      // Verify
      fd = open(fname, O_RDONLY);
      char buf[10];
      read(fd, buf, 10);
      close(fd);
      if (strncmp(buf, "ABCD", 4) != 0) return -1;
      return 0;
      
  } else if (strcmp(op, "pread") == 0) {
      int fd = open(fname, O_RDONLY);
      if (fd < 0) return -1;
      
      char buf[4];
      // Read 3 chars from offset 2 -> "234"
      ssize_t n = wali_pread(fd, buf, 3, 2);
      close(fd);
      
      if (n != 3) return -1;
      if (strncmp(buf, "234", 3) != 0) return -1;
      return 0;
      
  } else if (strcmp(op, "pwrite") == 0) {
      int fd = open(fname, O_WRONLY); // Not O_TRUNC, "0123456789"
      if (fd < 0) return -1;
      
      // Write "XY" at offset 2 -> "01XY45..."
      ssize_t n = wali_pwrite(fd, "XY", 2, 2);
      close(fd);
      
      if (n != 2) return -1;
      
      // Verify
      fd = open(fname, O_RDONLY);
      char buf[10];
      read(fd, buf, 10);
      close(fd);
      if (buf[2] != 'X' || buf[3] != 'Y') return -1;
      if (buf[0] != '0') return -1;
      return 0;
  }
  
  return -1;
}
