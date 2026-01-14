// CMD: setup="create /tmp/stat_target" args="/tmp/stat_target" cleanup="remove /tmp/stat_target"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_lstat")))
long long __imported_wali_lstat(const char *pathname, struct stat *statbuf);
int wali_lstat(const char *pathname, struct stat *statbuf) { return (int)__imported_wali_lstat(pathname, statbuf); }
#else
#include <sys/syscall.h>
int wali_lstat(const char *pathname, struct stat *statbuf) { return syscall(SYS_lstat, pathname, statbuf); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *path = argv[1];
  struct stat st;
  
  if (wali_lstat(path, &st) != 0) return -1;
  if (!S_ISREG(st.st_mode)) return -1;
  
  return 0;
}
