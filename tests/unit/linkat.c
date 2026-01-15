// CMD: setup="/tmp/linkat_src" args="/tmp/linkat_src /tmp/linkat_dst" cleanup="/tmp/linkat_src /tmp/linkat_dst"

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
__attribute__((__import_module__("wali"), __import_name__("SYS_linkat")))
long __imported_wali_linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_newfstatat")))
long __imported_wali_newfstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags);

int wali_linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) { 
    return (int)__imported_wali_linkat(olddirfd, oldpath, newdirfd, newpath, flags); 
}
int wali_newfstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) { 
    return (int)__imported_wali_newfstatat(dirfd, pathname, statbuf, flags); 
}

#else
#include <sys/syscall.h>
int wali_linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) { 
    return syscall(SYS_linkat, olddirfd, oldpath, newdirfd, newpath, flags); 
}
int wali_newfstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) { 
    return syscall(SYS_newfstatat, dirfd, pathname, statbuf, flags); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *src = argv[0];
    const char *dst = argv[1];
    
    // Create hard link
    if (wali_linkat(AT_FDCWD, src, AT_FDCWD, dst, 0) != 0) return -1;
    
    // Verify both exist
    struct stat st1, st2;
    if (wali_newfstatat(AT_FDCWD, src, &st1, 0) != 0) return -1;
    if (wali_newfstatat(AT_FDCWD, dst, &st2, 0) != 0) return -1;
    
    // Same inode
    if (st1.st_ino != st2.st_ino) return -1;
    
    // nlink should be 2
    if (st1.st_nlink != 2) return -1;
    
    return 0;
}
