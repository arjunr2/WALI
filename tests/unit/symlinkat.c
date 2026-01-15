// CMD: setup="/tmp/symlinkat_src" args="/tmp/symlinkat_src /tmp/symlinkat_dst" cleanup="/tmp/symlinkat_src /tmp/symlinkat_dst"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

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
__attribute__((__import_module__("wali"), __import_name__("SYS_symlinkat")))
long __imported_wali_symlinkat(const char *target, int newdirfd, const char *linkpath);
__attribute__((__import_module__("wali"), __import_name__("SYS_newfstatat")))
long __imported_wali_newfstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags);

int wali_symlinkat(const char *target, int newdirfd, const char *linkpath) { 
    return (int)__imported_wali_symlinkat(target, newdirfd, linkpath); 
}
int wali_newfstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) { 
    return (int)__imported_wali_newfstatat(dirfd, pathname, statbuf, flags); 
}

#else
#include <sys/syscall.h>
int wali_symlinkat(const char *target, int newdirfd, const char *linkpath) { 
    return syscall(SYS_symlinkat, target, newdirfd, linkpath); 
}
int wali_newfstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) { 
    return syscall(SYS_newfstatat, dirfd, pathname, statbuf, flags); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *target = argv[0];
    const char *linkpath = argv[1];
    
    // Create symlink
    if (wali_symlinkat(target, AT_FDCWD, linkpath) != 0) return -1;
    
    // Verify it's a symlink
    struct stat st;
    if (wali_newfstatat(AT_FDCWD, linkpath, &st, AT_SYMLINK_NOFOLLOW) != 0) return -1;
    
    if (!S_ISLNK(st.st_mode)) return -1;
    
    return 0;
}
