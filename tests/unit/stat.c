// CMD: setup="/tmp/stat_file.txt" args="file /tmp/stat_file.txt" cleanup="/tmp/stat_file.txt"
// CMD: args="fail /tmp/missing.txt"

#include "wali_start.c"
#include <fcntl.h>
#include <sys/stat.h>
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
    // Write 100 bytes
    char buf[100];
    memset(buf, 'x', 100);
    if (write(fd, buf, 100) != 100) {
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
// Import WALI syscalls
// We need headers for struct stat. <sys/stat.h> is included above.

__attribute__((__import_module__("wali"), __import_name__("SYS_stat")))
long __imported_wali_stat(const char *pathname, struct stat *statbuf);

__attribute__((__import_module__("wali"), __import_name__("SYS_fstat")))
long __imported_wali_fstat(int fd, struct stat *statbuf);

__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);

__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_stat(const char *pathname, struct stat *statbuf) {
  return (int) __imported_wali_stat(pathname, statbuf);
}
int wali_fstat(int fd, struct stat *statbuf) {
  return (int) __imported_wali_fstat(fd, statbuf);
}
int wali_open(const char *pathname, int flags, int mode) {
  return (int) __imported_wali_open(pathname, flags, mode);
}
int wali_close(int fd) {
  return (int) __imported_wali_close(fd);
}

#else
// Native
#include <unistd.h>
#include <sys/syscall.h>

int wali_stat(const char *pathname, struct stat *statbuf) {
  return syscall(SYS_stat, pathname, statbuf);
}
int wali_fstat(int fd, struct stat *statbuf) {
  return syscall(SYS_fstat, fd, statbuf);
}
int wali_open(const char *pathname, int flags, int mode) {
  return syscall(SYS_open, pathname, flags, mode);
}
int wali_close(int fd) {
  return syscall(SYS_close, fd);
}
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *mode = (argc > 0) ? argv[0] : "fail";
  
  struct stat st;
  memset(&st, 0, sizeof(st));
  
  if (strcmp(mode, "file") == 0) {
      if (argc < 2) return -1;
      const char *path = argv[1];
      
      // Test 1: stat
      if (wali_stat(path, &st) != 0) return -1;
      
      if (st.st_size != 100) return -1;
      if (!S_ISREG(st.st_mode)) return -1;
      
      // Test 2: fstat
      int fd = wali_open(path, O_RDONLY, 0);
      if (fd < 0) return -1;
      
      struct stat fst;
      if (wali_fstat(fd, &fst) != 0) {
          wali_close(fd);
          return -1;
      }
      wali_close(fd);
      
      if (fst.st_size != 100) return -1;
      if (!S_ISREG(fst.st_mode)) return -1;
      
      // Verify inode matches (usually good check)
      if (st.st_ino != fst.st_ino) return -1;
      
      return 0;
      
  } else if (strcmp(mode, "fail") == 0) {
      if (argc < 2) return -1;
      const char *path = argv[1];
      
      if (wali_stat(path, &st) == 0) return -1; // Should fail
      
      return 0;
  }
  
  return -1;
}
