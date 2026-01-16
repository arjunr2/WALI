// CMD: setup="/tmp/sub_chdir" args="test /tmp/sub_chdir" cleanup="/tmp/sub_chdir"

#include "wali_start.c"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <stddef.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    const char *dir = argv[0];
    rmdir(dir);
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    const char *dir = argv[0];
    rmdir(dir);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_mkdir")))
long __imported_wali_mkdir(const char *pathname, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_rmdir")))
long __imported_wali_rmdir(const char *pathname);
__attribute__((__import_module__("wali"), __import_name__("SYS_chdir")))
long __imported_wali_chdir(const char *pathname);
__attribute__((__import_module__("wali"), __import_name__("SYS_getcwd")))
long __imported_wali_getcwd(char *buf, size_t size);

int wali_mkdir(const char *pathname, int mode) { return (int)__imported_wali_mkdir(pathname, mode); }
int wali_rmdir(const char *pathname) { return (int)__imported_wali_rmdir(pathname); }
int wali_chdir(const char *pathname) { return (int)__imported_wali_chdir(pathname); }
int wali_getcwd(char *buf, unsigned long size) { return (int)__imported_wali_getcwd(buf, size); }

#else
#include <sys/syscall.h>
int wali_mkdir(const char *pathname, int mode) { return syscall(SYS_mkdir, pathname, mode); }
int wali_rmdir(const char *pathname) { return syscall(SYS_rmdir, pathname); }
int wali_chdir(const char *pathname) { return syscall(SYS_chdir, pathname); }
int wali_getcwd(char *buf, unsigned long size) { return syscall(SYS_getcwd, buf, size); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *dir = argv[1];
  
  char cwd1[512];
  char cwd2[512];
  
  if (wali_getcwd(cwd1, sizeof(cwd1)) <= 0) return -1;
  
  if (wali_mkdir(dir, 0755) != 0) return -1;
  
  if (wali_chdir(dir) != 0) return -1;
  
  if (wali_getcwd(cwd2, sizeof(cwd2)) <= 0) return -1;
  
  // verify cwd2 > cwd1 and contains dir
  if (strlen(cwd2) <= strlen(cwd1)) return -1;
  if (strstr(cwd2, dir) == NULL) return -1;
  
  if (wali_chdir("..") != 0) return -1;
  
  if (wali_rmdir(dir) != 0) return -1;
  
  return 0;
}
