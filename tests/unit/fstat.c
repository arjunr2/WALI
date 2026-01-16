// CMD: setup="/tmp/fstat_test" args="/tmp/fstat_test" cleanup="/tmp/fstat_test"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, "TEST", 4);
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
__attribute__((__import_module__("wali"), __import_name__("SYS_fstat")))
long __imported_wali_fstat(int fd, struct stat *statbuf);
__attribute__((__import_module__("wali"), __import_name__("SYS_openat")))
long __imported_wali_openat(int dirfd, const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_fstat(int fd, struct stat *statbuf) { return (int)__imported_wali_fstat(fd, statbuf); }
int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return (int)__imported_wali_openat(dirfd, pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_fstat(int fd, struct stat *statbuf) { return syscall(SYS_fstat, fd, statbuf); }
int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return syscall(SYS_openat, dirfd, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *path = argv[0];
    
    int fd = wali_openat(AT_FDCWD, path, O_RDONLY, 0);
    if (fd < 0) return -1;
    
    struct stat st;
    if (wali_fstat(fd, &st) != 0) {
        wali_close(fd);
        return -1;
    }
    
    // Check it's a regular file
    if (!S_ISREG(st.st_mode)) {
        wali_close(fd);
        return -1;
    }
    
    // Check size is 4 bytes
    if (st.st_size != 4) {
        wali_close(fd);
        return -1;
    }
    
    wali_close(fd);
    return 0;
}
