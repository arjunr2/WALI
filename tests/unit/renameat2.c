// CMD: setup="/tmp/renameat2_src" args="/tmp/renameat2_src /tmp/renameat2_dst" cleanup="remove /tmp/renameat2_src /tmp/renameat2_dst"

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
        write(fd, "DATA", 4);
        close(fd);
    }
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return -1;
    unlink(argv[0]);
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_renameat2")))
long __imported_wali_renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_faccessat")))
long __imported_wali_faccessat(int dirfd, const char *pathname, int mode, int flags);

int wali_renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) { 
    return (int)__imported_wali_renameat2(olddirfd, oldpath, newdirfd, newpath, flags); 
}
int wali_faccessat(int dirfd, const char *pathname, int mode, int flags) { 
    return (int)__imported_wali_faccessat(dirfd, pathname, mode, flags); 
}

#else
#include <sys/syscall.h>
int wali_renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) { 
    return syscall(SYS_renameat2, olddirfd, oldpath, newdirfd, newpath, flags); 
}
int wali_faccessat(int dirfd, const char *pathname, int mode, int flags) { 
    return syscall(SYS_faccessat, dirfd, pathname, mode, flags); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *src = argv[0];
    const char *dst = argv[1];
    
    // Rename with no flags
    if (wali_renameat2(AT_FDCWD, src, AT_FDCWD, dst, 0) != 0) return -1;
    
    // src should not exist
    if (wali_faccessat(AT_FDCWD, src, F_OK, 0) == 0) return -1;
    
    // dst should exist
    if (wali_faccessat(AT_FDCWD, dst, F_OK, 0) != 0) return -1;
    
    return 0;
}
