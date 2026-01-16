// CMD: setup="/tmp/pwrite_file" args="/tmp/pwrite_file" cleanup="/tmp/pwrite_file"

#define _GNU_SOURCE
#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    const char *fname = argv[0];
    int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, "0123456789", 10);
        close(fd);
    }
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_pwrite64")))
long long __imported_wali_pwrite64(int fd, const void *buf, size_t count, long long offset);
ssize_t wali_pwrite(int fd, const void *buf, size_t count, off_t offset) { return (ssize_t)__imported_wali_pwrite64(fd, buf, count, (long long)offset); }
#else
#include <sys/syscall.h>
ssize_t wali_pwrite(int fd, const void *buf, size_t count, off_t offset) { return syscall(SYS_pwrite64, fd, buf, count, offset); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    
    int fd = open(fname, O_WRONLY); // Not O_TRUNC, "0123456789"
    if (fd < 0) return -1;
    
    // Write "XY" at offset 2 -> "01XY45..."
    ssize_t n = wali_pwrite(fd, "XY", 2, 2);
    close(fd);
    
    if (n != 2) return -1;
    
    // Verify
    fd = open(fname, O_RDONLY);
    char buf[10];
    read(fd, buf, 10);
    close(fd);
    if (buf[2] != 'X' || buf[3] != 'Y') return -1;
    if (buf[0] != '0') return -1;
    return 0;
}
