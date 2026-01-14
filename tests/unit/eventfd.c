// CMD: args="eventfd"
// CMD: args="eventfd2"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_eventfd")))
long __imported_wali_eventfd(unsigned int initval);
__attribute__((__import_module__("wali"), __import_name__("SYS_eventfd2")))
long __imported_wali_eventfd2(unsigned int initval, int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_read")))
long __imported_wali_read(int fd, void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_eventfd(unsigned int initval) { return (int)__imported_wali_eventfd(initval); }
int wali_eventfd2(unsigned int initval, int flags) { return (int)__imported_wali_eventfd2(initval, flags); }
int wali_read(int fd, void *buf, size_t count) { return (int)__imported_wali_read(fd, buf, count); }
int wali_write(int fd, const void *buf, size_t count) { return (int)__imported_wali_write(fd, buf, count); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
#else
#include <sys/syscall.h>
int wali_eventfd(unsigned int initval) { return syscall(SYS_eventfd, initval); }
int wali_eventfd2(unsigned int initval, int flags) { return syscall(SYS_eventfd2, initval, flags); }
int wali_read(int fd, void *buf, size_t count) { return syscall(SYS_read, fd, buf, count); }
int wali_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

#ifndef EFD_CLOEXEC
#define EFD_CLOEXEC 02000000
#endif
#ifndef EFD_NONBLOCK
#define EFD_NONBLOCK 04000
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *mode = argv[0];

    int efd;
    if (strcmp(mode, "eventfd") == 0) {
        efd = wali_eventfd(10);
    } else {
        efd = wali_eventfd2(10, EFD_CLOEXEC | EFD_NONBLOCK);
    }

    if (efd < 0) return -1;

    uint64_t val;
    // Should be able to read the 10
    if (wali_read(efd, &val, sizeof(val)) != sizeof(val)) {
        wali_close(efd); return -1;
    }
    if (val != 10) { wali_close(efd); return -1; }

    // Now it's 0. Write 5.
    val = 5;
    if (wali_write(efd, &val, sizeof(val)) != sizeof(val)) {
        wali_close(efd); return -1;
    }

    // Read 5.
    if (wali_read(efd, &val, sizeof(val)) != sizeof(val)) {
        wali_close(efd); return -1;
    }
    if (val != 5) { wali_close(efd); return -1; }

    wali_close(efd);
    return 0;
}
