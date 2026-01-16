// CMD: setup="/tmp/fchmod.txt" args="/tmp/fchmod.txt" cleanup="/tmp/fchmod.txt"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    struct stat st;
    if (stat(argv[0], &st) != 0) {
        printf("Cleanup: stat failed for %s\n", argv[0]);
        return -1;
    }
    printf("Cleanup: mode is %o, expected 0600\n", st.st_mode & 0777);
    if ((st.st_mode & 0777) != 0600) return -1;
    
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_fchmod")))
long __imported_wali_fchmod(int fd, int mode);

__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);

__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_fchmod(int fd, int mode) { return (int)__imported_wali_fchmod(fd, mode); }
int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
#else
#include <sys/syscall.h>
int wali_fchmod(int fd, int mode) { return syscall(SYS_fchmod, fd, mode); }
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *path = argv[1];
    
    int fd = wali_open(path, O_RDONLY, 0);
    if (fd < 0) return -1;
    
    // Change permissions
    if (wali_fchmod(fd, 0600) != 0) {
        wali_close(fd);
        return -1;
    }
    
    // Check handled by fstat but we implicitly assume fchmod returning 0 worked.
    // Minimally we just test successful return.
    
    wali_close(fd);
    return 0;
}
