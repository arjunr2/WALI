// CMD: setup="/tmp/ren_src" args="/tmp/ren_src /tmp/ren_dst" cleanup="/tmp/ren_src /tmp/ren_dst"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    const char *src = argv[0];
    int fd = open(src, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, "DATA", 4);
        close(fd);
    }
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return -1;
    unlink(argv[0]);
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_rename")))
long long __imported_wali_rename(const char *oldpath, const char *newpath);
__attribute__((__import_module__("wali"), __import_name__("SYS_lstat")))
long long __imported_wali_lstat(const char *pathname, struct stat *statbuf);

int wali_rename(const char *oldpath, const char *newpath) { return (int)__imported_wali_rename(oldpath, newpath); }
int wali_lstat(const char *pathname, struct stat *statbuf) { return (int)__imported_wali_lstat(pathname, statbuf); }
#else
#include <sys/syscall.h>
int wali_rename(const char *oldpath, const char *newpath) { return syscall(SYS_rename, oldpath, newpath); }
int wali_lstat(const char *pathname, struct stat *statbuf) { return syscall(SYS_lstat, pathname, statbuf); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *src = argv[1];
  const char *dst = argv[2];
  
  if (wali_rename(src, dst) != 0) return -1;
  
  struct stat st;
  // src should be gone
  if (wali_lstat(src, &st) == 0) return -1;
  
  // dst should exist
  if (wali_lstat(dst, &st) != 0) return -1;
  
  return 0;
}
