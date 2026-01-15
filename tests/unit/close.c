// CMD: setup="create /tmp/close_test" args="/tmp/close_test" cleanup="remove /tmp/close_test"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return 0;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return 0;
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);
__attribute__((__import_module__("wali"), __import_name__("SYS_openat")))
long __imported_wali_openat(int dirfd, const char *pathname, int flags, int mode);

int wali_close(int fd) { return (int)__imported_wali_close(fd); }
int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return (int)__imported_wali_openat(dirfd, pathname, flags, mode); }

#else
#include <sys/syscall.h>
int wali_close(int fd) { return syscall(SYS_close, fd); }
int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return syscall(SYS_openat, dirfd, pathname, flags, mode); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *path = argv[0];
    
    int fd = wali_openat(AT_FDCWD, path, O_RDONLY, 0);
    if (fd < 0) return -1;
    
    // Close valid fd
    if (wali_close(fd) != 0) return -1;
    
    // Close already closed fd should fail
    int ret = wali_close(fd);
    if (ret == 0) return -1; // Should have failed
    
    return 0;
}
