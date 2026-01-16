// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>

#include <string.h>
#include <fcntl.h>

#define TEST_FILE "/tmp/dup3_test.txt"
#define TEST_CONTENT "DUP3"

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>
int test_setup(int argc, char **argv) {
    int fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return 0;
    write(fd, TEST_CONTENT, strlen(TEST_CONTENT));
    close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    unlink(TEST_FILE);
    return 0;
}
#endif

#ifdef __wasm__
WALI_IMPORT("SYS_dup3") long wali_syscall_dup3(int oldfd, int newfd, int flags);
#else
#include <sys/syscall.h>
long wali_syscall_dup3(int oldfd, int newfd, int flags) { return syscall(SYS_dup3, oldfd, newfd, flags); }
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
    int fd = wali_syscall_open(TEST_FILE, O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    int target_fd = 20;
    wali_syscall_close(target_fd); // ensure closed

    // dup3 with O_CLOEXEC
    long fd2 = wali_syscall_dup3(fd, target_fd, O_CLOEXEC);
    if (fd2 != target_fd) {
        wali_syscall_close(fd);
        TEST_FAIL("dup3 failed or returned wrong fd"); 
    }

    // Verify content
    char buf[10];
    memset(buf, 0, 10);
    int len = strlen(TEST_CONTENT);
    if (wali_syscall_read(fd2, buf, len) != len) { 
        wali_syscall_close(fd); wali_syscall_close(fd2);
        TEST_FAIL("read failed");
    }
    if (strncmp(buf, TEST_CONTENT, len) != 0) {
        wali_syscall_close(fd); wali_syscall_close(fd2);
        TEST_FAIL("Content mismatch");
    }

    // Verify CLOEXEC flag set
    long flags = wali_syscall_fcntl(fd2, F_GETFD, 0);
    if (flags < 0) {
        wali_syscall_close(fd); wali_syscall_close(fd2);
        TEST_FAIL("fcntl failed");
    }
    if (!(flags & FD_CLOEXEC)) {
         wali_syscall_close(fd); wali_syscall_close(fd2);
         TEST_FAIL("FD_CLOEXEC not set");
    }

    wali_syscall_close(fd);
    wali_syscall_close(fd2);
    return 0;
}

