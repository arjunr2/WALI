// CMD: setup="/tmp/umask.txt" args="/tmp/umask.txt" cleanup="/tmp/umask.txt"

#include "wali_start.c"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    // Ensure file doesn't exist
    if (argc < 1) return -1;
    unlink(argv[0]);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    // Verify file exists and has correct mode
    struct stat st;
    if (stat(argv[0], &st) != 0) return -1;
    
    // Created with 0666, umask 044 (00100100)
    // Result should be 0622 (rw- -w- -w-) theoretically?
    // Wait, 0666 is 110 110 110. 044 is 000 100 100.
    // ~044 is 111 011 011.
    // 0666 & ~044 = 110 010 010 = 0622.
    
    if ((st.st_mode & 0777) != 0622) return -1;

    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_umask")))
long __imported_wali_umask(int mask);
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_umask(int mask) { return (int)__imported_wali_umask(mask); }
int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
#else
#include <sys/syscall.h>
int wali_umask(int mask) { return syscall(SYS_umask, mask); }
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *path = argv[1];
    
    // Set umask to 044
    wali_umask(044);
    
    // Create file with 0666
    int fd = wali_open(path, O_CREAT | O_WRONLY, 0666);
    if (fd < 0) return -1;
    
    wali_close(fd);
    
    return 0;
}
