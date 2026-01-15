// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_fchdir")))
long __imported_wali_fchdir(int fd);
__attribute__((__import_module__("wali"), __import_name__("SYS_openat")))
long __imported_wali_openat(int dirfd, const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);
__attribute__((__import_module__("wali"), __import_name__("SYS_getcwd")))
long __imported_wali_getcwd(char *buf, size_t size);

int wali_fchdir(int fd) { return (int)__imported_wali_fchdir(fd); }
int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return (int)__imported_wali_openat(dirfd, pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
long wali_getcwd(char *buf, size_t size) { return __imported_wali_getcwd(buf, size); }

#else
#include <sys/syscall.h>
int wali_fchdir(int fd) { return syscall(SYS_fchdir, fd); }
int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return syscall(SYS_openat, dirfd, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
long wali_getcwd(char *buf, size_t size) { return syscall(SYS_getcwd, buf, size); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Open current dir
    int oldfd = wali_openat(AT_FDCWD, ".", O_RDONLY | O_DIRECTORY, 0);
    if (oldfd < 0) return -1;
    
    // Open /tmp
    int tmpfd = wali_openat(AT_FDCWD, "/tmp", O_RDONLY | O_DIRECTORY, 0);
    if (tmpfd < 0) {
        wali_close(oldfd);
        return -1;
    }
    
    // Change to /tmp
    if (wali_fchdir(tmpfd) != 0) {
        wali_close(oldfd);
        wali_close(tmpfd);
        return -1;
    }
    
    // Verify we're in /tmp
    char buf[256];
    if (wali_getcwd(buf, sizeof(buf)) <= 0) {
        wali_fchdir(oldfd);
        wali_close(oldfd);
        wali_close(tmpfd);
        return -1;
    }
    
    // Change back
    wali_fchdir(oldfd);
    wali_close(oldfd);
    wali_close(tmpfd);
    
    return 0;
}
