// CMD: setup="create /tmp/ioctl_test" args="basic /tmp/ioctl_test"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_ioctl")))
long __imported_wali_ioctl(int fd, unsigned long request, void *arg);
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_ioctl(int fd, unsigned long request, void *arg) { return (int)__imported_wali_ioctl(fd, request, arg); }
int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
#else
#include <sys/syscall.h>
int wali_ioctl(int fd, unsigned long request, void *arg) { return syscall(SYS_ioctl, fd, request, arg); }
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    if (argc < 2) return -1;
    
    // Testing ioctl is tricky without a device. 
    // We can test on a regular file, which usually fails with ENOTTY.
    
    int fd = wali_open(argv[1], O_RDONLY, 0);
    if (fd < 0) return -1;
    
    int dummy = 0;
    // FIOCLEX might be supported? checking...
    // Or just check that it returns error -1 and errno matches native?
    // Note: WALI/WASM wrapper might not set errno same way.
    // For this test, we just check that the syscall goes through and returns the same code (likely -1).
    // Native: ioctl on file -> -1 (ENOTTY)
    
    int res = wali_ioctl(fd, TCGETS, &dummy);
    
    // Result should be -1 for regular file
    if (res != -1) {
        wali_close(fd);
        return -1;
    }
    
    // If we're lucky to have a tty (0/1/2), we could test TCGETS there, but might be inconsistent in test env.
    
    wali_close(fd);
    return 0;
}
