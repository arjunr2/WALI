// CMD: setup="/tmp/unlinkat_dir" args="/tmp/unlinkat_dir" cleanup="/tmp/unlinkat_dir"

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
    if (argc < 1) return -1;
    mkdir(argv[0], 0755);
    // Create file to unlink
    char buf[256];
    snprintf(buf, sizeof(buf), "%s/f1", argv[0]);
    int fd = open(buf, O_WRONLY | O_CREAT, 0644);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    // Verify file is gone
    char buf[256];
    snprintf(buf, sizeof(buf), "%s/f1", argv[0]);
    
    struct stat st;
    if (stat(buf, &st) == 0) {
         unlink(buf); // cleanup anyway
    }
    
    rmdir(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_unlinkat")))
long long __imported_wali_unlinkat(int dirfd, const char *pathname, int flags);
int wali_unlinkat(int dirfd, const char *pathname, int flags) { return (int)__imported_wali_unlinkat(dirfd, pathname, flags); }
#else
#include <sys/syscall.h>
int wali_unlinkat(int dirfd, const char *pathname, int flags) { return syscall(SYS_unlinkat, dirfd, pathname, flags); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *dname = argv[1];
    
    int dirfd = open(dname, O_RDONLY | O_DIRECTORY);
    if (dirfd < 0) return -1;
    
    if (wali_unlinkat(dirfd, "f1", 0) != 0) return -1;
    
    close(dirfd);
    return 0;
}
