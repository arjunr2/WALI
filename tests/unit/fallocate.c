// CMD: setup="create /tmp/fa_file" args="fallocate /tmp/fa_file"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_fallocate")))
long long __imported_wali_fallocate(int fd, int mode, long long offset, long long len);

__attribute__((__import_module__("wali"), __import_name__("SYS_fadvise64")))
long long __imported_wali_fadvise64(int fd, long long offset, long long len, int advice);

int wali_fallocate(int fd, int mode, off_t offset, off_t len) { 
    return (int)__imported_wali_fallocate(fd, mode, offset, len); 
}
int wali_fadvise(int fd, off_t offset, off_t len, int advice) {
    return (int)__imported_wali_fadvise64(fd, offset, len, advice);
}
#else
#include <sys/syscall.h>
// SYS_fallocate 285
// SYS_fadvise64 221
int wali_fallocate(int fd, int mode, off_t offset, off_t len) { 
    return syscall(SYS_fallocate, fd, mode, offset, len); 
}
int wali_fadvise(int fd, off_t offset, off_t len, int advice) {
    return syscall(SYS_fadvise64, fd, offset, len, advice);
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    int fd = open(fname, O_RDWR);
    if (fd < 0) return -1;
    
    // Allocate 100 bytes
    if (wali_fallocate(fd, 0, 0, 100) != 0) {
        // If not supported, we might return 0 if ignored, or error.
        // If error, check errno? Syscall wrapper returns -1 usually and sets errno.
        // My wrapper returns ret code directly (from import).
        // Negative for error?
        // But native wrapper uses syscall() which returns -1.
        // Let's relax check if EOPNOTSUPP?
        // But we expect WALI to implement it or proxy it.
        return -1;
    }
    
    struct stat st;
    if (fstat(fd, &st) != 0) return -1;
    if (st.st_size != 100) return -1;
    
    // fadvise
    if (wali_fadvise(fd, 0, 100, 1) != 0) { // POSIX_FADV_RANDOM = 1
        // Usually always succeeds on linux even if ignored
        return -1;
    }
    
    close(fd);
    return 0;
}
