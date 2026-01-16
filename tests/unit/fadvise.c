// CMD: setup="/tmp/fadvise_test" args="basic /tmp/fadvise_test" cleanup="/tmp/fadvise_test"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_fadvise")))
long __imported_wali_fadvise(int fd, long long offset, long long len, int advice);
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_fadvise(int fd, long long offset, long long len, int advice) { return (int)__imported_wali_fadvise(fd, offset, len, advice); }
int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
#else
#include <sys/syscall.h>
int wali_fadvise(int fd, long long offset, long long len, int advice) { return syscall(SYS_fadvise64, fd, offset, len, advice); }
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

#ifndef POSIX_FADV_NORMAL
#define POSIX_FADV_NORMAL 0
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    if (argc < 2) return -1;
    
    int fd = wali_open(argv[1], O_RDONLY, 0);
    if (fd < 0) return -1;
    
    if (wali_fadvise(fd, 0, 0, POSIX_FADV_NORMAL) != 0) {
        wali_close(fd);
        return -1;
    }
    
    wali_close(fd);
    return 0;
}
