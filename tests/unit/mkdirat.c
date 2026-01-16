// CMD: setup="/tmp/mkdirat_dir" args="/tmp/mkdirat_dir" cleanup="/tmp/mkdirat_dir"

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
    snprintf(buf, sizeof(buf), "%s/sub", argv[0]);
    
    // Verification
    struct stat st;
    if (stat(buf, &st) != 0) {
        rmdir(argv[0]);
    }
    
    rmdir(buf);
    rmdir(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_mkdirat")))
long long __imported_wali_mkdirat(int dirfd, const char *pathname, int mode);
int wali_mkdirat(int dirfd, const char *pathname, int mode) { return (int)__imported_wali_mkdirat(dirfd, pathname, mode); }
#else
#include <sys/syscall.h>
int wali_mkdirat(int dirfd, const char *pathname, int mode) { return syscall(SYS_mkdirat, dirfd, pathname, mode); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *dname = argv[1];
    
    int dirfd = open(dname, O_RDONLY | O_DIRECTORY);
    if (dirfd < 0) return -1;
    
    if (wali_mkdirat(dirfd, "sub", 0755) != 0) return -1;
    
    close(dirfd);
    return 0;
}
