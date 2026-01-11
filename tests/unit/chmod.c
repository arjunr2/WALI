// CMD: setup="create /tmp/chmod.txt" args="file /tmp/chmod.txt"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *path = argv[1];
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_chmod")))
long __imported_wali_chmod(const char *pathname, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_fchmod")))
long __imported_wali_fchmod(int fd, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_stat")))
long __imported_wali_stat(const char *pathname, struct stat *statbuf);
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_chmod(const char *pathname, int mode) { return (int)__imported_wali_chmod(pathname, mode); }
int wali_fchmod(int fd, int mode) { return (int)__imported_wali_fchmod(fd, mode); }
int wali_stat(const char *pathname, struct stat *statbuf) { return (int)__imported_wali_stat(pathname, statbuf); }
int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_chmod(const char *pathname, int mode) { return syscall(SYS_chmod, pathname, mode); }
int wali_fchmod(int fd, int mode) { return syscall(SYS_fchmod, fd, mode); }
int wali_stat(const char *pathname, struct stat *statbuf) { return syscall(SYS_stat, pathname, statbuf); }
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *path = argv[1];
  
  // 1. chmod to 0777
  if (wali_chmod(path, 0777) != 0) return -1;
  
  struct stat st;
  if (wali_stat(path, &st) != 0) return -1;
  if ((st.st_mode & 0777) != 0777) return -1;
  
  // 2. fchmod to 0600
  int fd = wali_open(path, O_RDONLY, 0);
  if (fd < 0) return -1;
  
  if (wali_fchmod(fd, 0600) != 0) { wali_close(fd); return -1; }
  wali_close(fd);
  
  if (wali_stat(path, &st) != 0) return -1;
  if ((st.st_mode & 0777) != 0600) return -1;
  
  return 0;
}
