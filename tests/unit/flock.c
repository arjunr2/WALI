// CMD: setup="/tmp/flock.lock" args="lock /tmp/flock.lock" cleanup="/tmp/flock.lock"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
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
__attribute__((__import_module__("wali"), __import_name__("SYS_flock")))
long __imported_wali_flock(int fd, int operation);
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_flock(int fd, int operation) { return (int)__imported_wali_flock(fd, operation); }
int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
#else
#include <sys/syscall.h>
int wali_flock(int fd, int operation) { return syscall(SYS_flock, fd, operation); }
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    if (argc < 2) return -1;
    
    int fd = wali_open(argv[1], O_RDWR, 0);
    if (fd < 0) return -1;

    // Exclusive lock
    if (wali_flock(fd, LOCK_EX) != 0) {
        wali_close(fd); return -1;
    }

    // Unlock
    if (wali_flock(fd, LOCK_UN) != 0) {
        wali_close(fd); return -1;
    }

    // Shared lock
    if (wali_flock(fd, LOCK_SH) != 0) {
        wali_close(fd); return -1;
    }

    wali_close(fd);
    return 0;
}
