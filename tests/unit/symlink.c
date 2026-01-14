// CMD: setup="create /tmp/sym_src_sl" args="/tmp/sym_src_sl /tmp/sym_dst_sl" cleanup="remove /tmp/sym_src_sl /tmp/sym_dst_sl"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *src = argv[1];
    int fd = open(src, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, "DATA", 4);
        close(fd);
    }
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 3) return 0;
    unlink(argv[1]);
    unlink(argv[2]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_symlink")))
long long __imported_wali_symlink(const char *target, const char *linkpath);
__attribute__((__import_module__("wali"), __import_name__("SYS_readlink")))
long long __imported_wali_readlink(const char *pathname, char *buf, size_t bufsiz);
__attribute__((__import_module__("wali"), __import_name__("SYS_lstat")))
long long __imported_wali_lstat(const char *pathname, struct stat *statbuf);

int wali_symlink(const char *target, const char *linkpath) { return (int)__imported_wali_symlink(target, linkpath); }
ssize_t wali_readlink(const char *pathname, char *buf, size_t bufsiz) { return (ssize_t)__imported_wali_readlink(pathname, buf, bufsiz); }
int wali_lstat(const char *pathname, struct stat *statbuf) { return (int)__imported_wali_lstat(pathname, statbuf); }
#else
#include <sys/syscall.h>
int wali_symlink(const char *target, const char *linkpath) { return syscall(SYS_symlink, target, linkpath); }
ssize_t wali_readlink(const char *pathname, char *buf, size_t bufsiz) { return syscall(SYS_readlink, pathname, buf, bufsiz); }
int wali_lstat(const char *pathname, struct stat *statbuf) { return syscall(SYS_lstat, pathname, statbuf); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *src = argv[1];
  const char *dst = argv[2];
  
  if (wali_symlink(src, dst) != 0) return -1;
  
  char buf[64];
  ssize_t len = wali_readlink(dst, buf, 64);
  if (len < 0) return -1;
  buf[len] = '\0';
  
  if (strcmp(buf, src) != 0) return -1;

  struct stat st;
  if (wali_lstat(dst, &st) != 0) return -1;
  if (!S_ISLNK(st.st_mode)) return -1;

  return 0;
}
