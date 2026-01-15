// CMD: args="basic"

#include "wali_start.c"
#include <sys/eventfd.h>
#include <unistd.h>
#include <stdint.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_eventfd2")))
long __imported_wali_eventfd2(unsigned int initval, int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_read")))
long __imported_wali_read(int fd, void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_eventfd2(unsigned int initval, int flags) { return (int)__imported_wali_eventfd2(initval, flags); }
ssize_t wali_write(int fd, const void *buf, size_t count) { return (ssize_t)__imported_wali_write(fd, buf, count); }
ssize_t wali_read(int fd, void *buf, size_t count) { return (ssize_t)__imported_wali_read(fd, buf, count); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_eventfd2(unsigned int initval, int flags) { return syscall(SYS_eventfd2, initval, flags); }
ssize_t wali_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
ssize_t wali_read(int fd, void *buf, size_t count) { return syscall(SYS_read, fd, buf, count); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Create with non-blocking flag
    int efd = wali_eventfd2(0, EFD_NONBLOCK);
    if (efd < 0) return -1;
    
    // Write value
    uint64_t val = 5;
    if (wali_write(efd, &val, sizeof(val)) != sizeof(val)) {
        wali_close(efd);
        return -1;
    }
    
    // Read value
    uint64_t rval = 0;
    if (wali_read(efd, &rval, sizeof(rval)) != sizeof(rval)) {
        wali_close(efd);
        return -1;
    }
    
    if (rval != 5) {
        wali_close(efd);
        return -1;
    }
    
    wali_close(efd);
    return 0;
}
