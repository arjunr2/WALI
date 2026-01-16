// CMD: setup="/tmp/openat_dir" args="/tmp/openat_dir" cleanup="/tmp/openat_dir"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    mkdir(argv[0], 0755);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    char buf[256];
    snprintf(buf, sizeof(buf), "%s/f1", argv[0]);
    
    // Verification
    struct stat st;
    if (stat(buf, &st) != 0) {
        rmdir(argv[0]);
    }
    
    unlink(buf);
    rmdir(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_openat")))
long long __imported_wali_openat(int dirfd, const char *pathname, int flags, int mode);
int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return (int)__imported_wali_openat(dirfd, pathname, flags, mode); }
#else
#include <sys/syscall.h>
int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return syscall(SYS_openat, dirfd, pathname, flags, mode); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *dname = argv[1];
    
    int dirfd = open(dname, O_RDONLY | O_DIRECTORY);
    if (dirfd < 0) return -1;
    
    // Create f1 inside dname using openat relative
    int fd = wali_openat(dirfd, "f1", O_WRONLY | O_CREAT, 0644);
    if (fd < 0) return -1;
    
    close(fd);
    close(dirfd);
    return 0;
}
