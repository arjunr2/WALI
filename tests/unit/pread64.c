// CMD: setup="create /tmp/pread_file" args="/tmp/pread_file"

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
__attribute__((__import_module__("wali"), __import_name__("SYS_pread64")))
long long __imported_wali_pread64(int fd, void *buf, size_t count, long long offset);
ssize_t wali_pread(int fd, void *buf, size_t count, off_t offset) { return (ssize_t)__imported_wali_pread64(fd, buf, count, (long long)offset); }
#else
#include <sys/syscall.h>
ssize_t wali_pread(int fd, void *buf, size_t count, off_t offset) { return syscall(SYS_pread64, fd, buf, count, offset); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    
    int fd = open(fname, O_RDONLY);
    if (fd < 0) return -1;
    
    char buf[4];
    // Read 3 chars from offset 2 -> "234"
    ssize_t n = wali_pread(fd, buf, 3, 2);
    close(fd);
    
    if (n != 3) return -1;
    if (strncmp(buf, "234", 3) != 0) return -1;
    return 0;
}
