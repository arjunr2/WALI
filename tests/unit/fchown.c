// CMD: setup="/tmp/fchown.txt" args="/tmp/fchown.txt" cleanup="/tmp/fchown.txt"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    
    struct stat st;
    if (stat(argv[0], &st) != 0) return -1;
    if (st.st_uid != getuid()) return -1;
    if (st.st_gid != getgid()) return -1;
    
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_fchown")))
long __imported_wali_fchown(int fd, int owner, int group);

__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);

__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_fchown(int fd, int owner, int group) { return (int)__imported_wali_fchown(fd, owner, group); }
int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
#else
#include <sys/syscall.h>
int wali_fchown(int fd, int owner, int group) { return syscall(SYS_fchown, fd, owner, group); }
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *path = argv[1];
    
    int fd = wali_open(path, O_RDONLY, 0);
    if (fd < 0) return -1;
    
    // Test safe no-op (-1, -1)
    if (wali_fchown(fd, -1, -1) != 0) {
        wali_close(fd);
        return -1;
    }
    
    wali_close(fd);
    return 0;
}
