// CMD: setup="create /tmp/readv_file" args="/tmp/readv_file"

#define _GNU_SOURCE
#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *fname = argv[1];
    int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, "0123456789", 10);
        close(fd);
    }
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_readv")))
long long __imported_wali_readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t wali_readv(int fd, const struct iovec *iov, int iovcnt) { return (ssize_t)__imported_wali_readv(fd, iov, iovcnt); }
#else
#include <sys/syscall.h>
ssize_t wali_readv(int fd, const struct iovec *iov, int iovcnt) { return syscall(SYS_readv, fd, iov, iovcnt); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    
    int fd = open(fname, O_RDONLY);
    if (fd < 0) return -1;
    
    char buf1[3]; // "012"
    char buf2[3]; // "345"
    struct iovec iov[2];
    iov[0].iov_base = buf1;
    iov[0].iov_len = 3;
    iov[1].iov_base = buf2;
    iov[1].iov_len = 3;
    
    ssize_t n = wali_readv(fd, iov, 2);
    close(fd);
    
    if (n != 6) return -1;
    if (strncmp(buf1, "012", 3) != 0) return -1;
    if (strncmp(buf2, "345", 3) != 0) return -1;
    return 0;
}
