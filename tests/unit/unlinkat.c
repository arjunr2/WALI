// CMD: setup="create_dir /tmp/unlinkat_dir" args="/tmp/unlinkat_dir"

#define _GNU_SOURCE
#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR 0x200
#endif

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    mkdir(argv[1], 0755);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    // Just in case it failed to unlink
    char buf[256];
    snprintf(buf, sizeof(buf), "%s/f1", argv[1]);
    unlink(buf);
    rmdir(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_unlinkat")))
long long __imported_wali_unlinkat(int dirfd, const char *pathname, int flags);
int wali_unlinkat(int dirfd, const char *pathname, int flags) { return (int)__imported_wali_unlinkat(dirfd, pathname, flags); }

// Need openat for setup part of the test inside Wasm
__attribute__((__import_module__("wali"), __import_name__("SYS_openat")))
long long __imported_wali_openat(int dirfd, const char *pathname, int flags, int mode);
int wali_openat_helper(int dirfd, const char *pathname, int flags, int mode) { return (int)__imported_wali_openat(dirfd, pathname, flags, mode); }

#else
#include <sys/syscall.h>
int wali_unlinkat(int dirfd, const char *pathname, int flags) { return syscall(SYS_unlinkat, dirfd, pathname, flags); }
int wali_openat_helper(int dirfd, const char *pathname, int flags, int mode) { return syscall(SYS_openat, dirfd, pathname, flags, mode); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *dname = argv[1];
    
    int dirfd = open(dname, O_RDONLY | O_DIRECTORY);
    if (dirfd < 0) return -1;
    
    // Pre-create file to unlink
    int fd = wali_openat_helper(dirfd, "f1", O_WRONLY | O_CREAT, 0644);
    if (fd >= 0) close(fd);
    
    if (wali_unlinkat(dirfd, "f1", 0) != 0) return -1;
    
    struct stat st;
    char buf[256];
    snprintf(buf, sizeof(buf), "%s/f1", dname);
    if (stat(buf, &st) == 0) return -1; // Should fail as it's gone
    
    close(dirfd);
    return 0;
}
