// CMD: setup="/tmp/dup3_test.txt" args="file /tmp/dup3_test.txt" cleanup="/tmp/dup3_test.txt"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return 0;
    write(fd, "DUP3", 4);
    close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_read")))
long __imported_wali_read(int fd, void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);
__attribute__((__import_module__("wali"), __import_name__("SYS_dup3")))
long __imported_wali_dup3(int oldfd, int newfd, int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_fcntl")))
long __imported_wali_fcntl(int fd, int cmd, unsigned long arg);

int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_read(int fd, void *buf, size_t count) { return (int)__imported_wali_read(fd, buf, count); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
int wali_dup3(int oldfd, int newfd, int flags) { return (int)__imported_wali_dup3(oldfd, newfd, flags); }
int wali_fcntl(int fd, int cmd, unsigned long arg) { return (int)__imported_wali_fcntl(fd, cmd, arg); }
#else
#include <sys/syscall.h>
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_read(int fd, void *buf, size_t count) { return syscall(SYS_read, fd, buf, count); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
int wali_dup3(int oldfd, int newfd, int flags) { return syscall(SYS_dup3, oldfd, newfd, flags); }
int wali_fcntl(int fd, int cmd, unsigned long arg) { return syscall(SYS_fcntl, fd, cmd, arg); }
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC 02000000
#endif
#ifndef FD_CLOEXEC
#define FD_CLOEXEC 1
#endif
#ifndef F_GETFD
#define F_GETFD 1
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    if (argc < 2) return -1;
    const char *path = argv[1];

    int fd = wali_open(path, O_RDONLY, 0);
    if (fd < 0) return -1;

    int target_fd = 20;
    wali_close(target_fd); // ensure closed

    // dup3 with O_CLOEXEC
    int fd2 = wali_dup3(fd, target_fd, O_CLOEXEC);
    if (fd2 != target_fd) {
        wali_close(fd);
        return -1; 
    }

    // Verify content
    char buf[10];
    memset(buf, 0, 10);
    if (wali_read(fd2, buf, 4) != 4) return -1;
    if (strncmp(buf, "DUP3", 4) != 0) return -1;

    // Verify CLOEXEC flag set
    int flags = wali_fcntl(fd2, F_GETFD, 0);
    if (flags < 0) return -1;
    if (!(flags & FD_CLOEXEC)) return -1; // Must have cloexec

    wali_close(fd);
    wali_close(fd2);
    return 0;
}
