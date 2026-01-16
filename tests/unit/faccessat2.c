// CMD: setup="/tmp/faccessat2_test" args="exists /tmp/faccessat2_test" cleanup="/tmp/faccessat2_test"
// CMD: args="not_exists /tmp/faccessat2_noexist"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return 0;
    const char *path = argv[0];
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return 0;
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_faccessat2")))
long __imported_wali_faccessat2(int dirfd, const char *pathname, int mode, int flags);

int wali_faccessat2(int dirfd, const char *pathname, int mode, int flags) { 
    return (int)__imported_wali_faccessat2(dirfd, pathname, mode, flags); 
}

#else
#include <sys/syscall.h>
int wali_faccessat2(int dirfd, const char *pathname, int mode, int flags) { 
    return syscall(SYS_faccessat2, dirfd, pathname, mode, flags); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *mode = argv[0];
    const char *path = argv[1];
    
    if (strcmp(mode, "exists") == 0) {
        // File should exist, use AT_EACCESS flag
        if (wali_faccessat2(AT_FDCWD, path, F_OK, 0) != 0) return -1;
        if (wali_faccessat2(AT_FDCWD, path, R_OK, AT_EACCESS) != 0) return -1;
    } else if (strcmp(mode, "not_exists") == 0) {
        if (wali_faccessat2(AT_FDCWD, path, F_OK, 0) == 0) return -1;
    } else {
        return -1;
    }
    
    return 0;
}
