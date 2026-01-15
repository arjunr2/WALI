// CMD: setup="create_dir /tmp/rmdir_test" args="/tmp/rmdir_test" cleanup="remove_dir /tmp/rmdir_test"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return 0;
    mkdir(argv[0], 0755);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return 0;
    rmdir(argv[0]); // May already be removed by test
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_rmdir")))
long __imported_wali_rmdir(const char *pathname);
__attribute__((__import_module__("wali"), __import_name__("SYS_faccessat")))
long __imported_wali_faccessat(int dirfd, const char *pathname, int mode, int flags);

int wali_rmdir(const char *pathname) { return (int)__imported_wali_rmdir(pathname); }
int wali_faccessat(int dirfd, const char *pathname, int mode, int flags) { 
    return (int)__imported_wali_faccessat(dirfd, pathname, mode, flags); 
}

#else
#include <sys/syscall.h>
int wali_rmdir(const char *pathname) { return syscall(SYS_rmdir, pathname); }
int wali_faccessat(int dirfd, const char *pathname, int mode, int flags) { 
    return syscall(SYS_faccessat, dirfd, pathname, mode, flags); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *path = argv[0];
    
    // Remove directory
    if (wali_rmdir(path) != 0) return -1;
    
    // Should not exist anymore
    if (wali_faccessat(AT_FDCWD, path, F_OK, 0) == 0) return -1;
    
    return 0;
}
