// CMD: setup="create /tmp/link_src" args="link /tmp/link_src /tmp/link_dst" cleanup="remove /tmp/link_src /tmp/link_dst"
// CMD: setup="create /tmp/sym_src" args="symlink /tmp/sym_src /tmp/sym_dst" cleanup="remove /tmp/sym_src /tmp/sym_dst"
// CMD: setup="create /tmp/ren_src" args="rename /tmp/ren_src /tmp/ren_dst" cleanup="remove /tmp/ren_src /tmp/ren_dst"

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
    const char *mode = argv[0];
    const char *src = argv[1];
    
    if (strcmp(mode, "create") == 0) {
        int fd = open(src, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd >= 0) {
            write(fd, "DATA", 4);
            close(fd);
        }
    }
    return 0;
}

int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0; 
    const char *mode = argv[0];
    
    // Cleanup directive: cleanup="remove f1 f2 ..."
    if (strcmp(mode, "remove") == 0) {
        for (int i = 1; i < argc; i++) {
            unlink(argv[i]);
        }
    } else if (strcmp(mode, "create") == 0) {
        // Fallback for setup args if cleanup not specified (though we use explicit cleanup now)
        unlink(argv[1]);
    }
    
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_link")))
long long __imported_wali_link(const char *oldpath, const char *newpath);
__attribute__((__import_module__("wali"), __import_name__("SYS_symlink")))
long long __imported_wali_symlink(const char *target, const char *linkpath);
__attribute__((__import_module__("wali"), __import_name__("SYS_readlink")))
long long __imported_wali_readlink(const char *pathname, char *buf, size_t bufsiz);
__attribute__((__import_module__("wali"), __import_name__("SYS_rename")))
long long __imported_wali_rename(const char *oldpath, const char *newpath);
__attribute__((__import_module__("wali"), __import_name__("SYS_lstat")))
long long __imported_wali_lstat(const char *pathname, struct stat *statbuf);

int wali_link(const char *oldpath, const char *newpath) { return (int)__imported_wali_link(oldpath, newpath); }
int wali_symlink(const char *target, const char *linkpath) { return (int)__imported_wali_symlink(target, linkpath); }
ssize_t wali_readlink(const char *pathname, char *buf, size_t bufsiz) { return (ssize_t)__imported_wali_readlink(pathname, buf, bufsiz); }
int wali_rename(const char *oldpath, const char *newpath) { return (int)__imported_wali_rename(oldpath, newpath); }
int wali_lstat(const char *pathname, struct stat *statbuf) { return (int)__imported_wali_lstat(pathname, statbuf); }

#else
#include <sys/syscall.h>
int wali_link(const char *oldpath, const char *newpath) { return syscall(SYS_link, oldpath, newpath); }
int wali_symlink(const char *target, const char *linkpath) { return syscall(SYS_symlink, target, linkpath); }
ssize_t wali_readlink(const char *pathname, char *buf, size_t bufsiz) { return syscall(SYS_readlink, pathname, buf, bufsiz); }
int wali_rename(const char *oldpath, const char *newpath) { return syscall(SYS_rename, oldpath, newpath); }
int wali_lstat(const char *pathname, struct stat *statbuf) { return syscall(SYS_lstat, pathname, statbuf); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  if (argc < 3) return -1; // link src dst
  
  const char *op = argv[0];
  const char *src = argv[1];
  const char *dst = argv[2];
  
  if (strcmp(op, "link") == 0) {
      if (wali_link(src, dst) != 0) return -1;
      // Check hard link count using lstat or stat
      struct stat st;
      if (wali_lstat(src, &st) != 0) return -1;
      if (st.st_nlink != 2) return -1;
      
      if (wali_lstat(dst, &st) != 0) return -1;
      if (st.st_nlink != 2) return -1;
      
      // Cleanup
      // wali_unlink(dst); // Not imported in this file, use wrapper cleanup or import
      return 0;
      
  } else if (strcmp(op, "symlink") == 0) {
      if (wali_symlink(src, dst) != 0) return -1;
      
      char buf[64];
      memset(buf, 0, sizeof(buf));
      ssize_t len = wali_readlink(dst, buf, sizeof(buf));
      if (len != strlen(src)) return -1;
      if (strncmp(buf, src, len) != 0) return -1;
      
      struct stat st;
      if (wali_lstat(dst, &st) != 0) return -1;
      if (!S_ISLNK(st.st_mode)) return -1;
      
      return 0;
      
  } else if (strcmp(op, "rename") == 0) {
      if (wali_rename(src, dst) != 0) return -1;
      
      // src should be gone, dst should exist
      struct stat st;
      if (wali_lstat(src, &st) == 0) return -1; // Should fail
      if (wali_lstat(dst, &st) != 0) return -1; // Should succeed
      
      return 0;
  }
  
  return -1;
}
