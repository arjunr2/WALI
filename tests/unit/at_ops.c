// CMD: setup="create_dir /tmp/dat1" args="openat /tmp/dat1"
// CMD: setup="create_dir /tmp/dat2" args="mkdirat /tmp/dat2"
// CMD: setup="create_dir /tmp/dat3" args="unlinkat /tmp/dat3"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR 0x200
#endif

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *dname = argv[1];
    mkdir(dname, 0755);
    
    // For unlinkat, create a file inside
    if (strcmp(argv[0], "create_dir") == 0) { 
        // Always creating dir
    }
    
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *dname = argv[1];
    char buf[256];
    snprintf(buf, sizeof(buf), "%s/f1", dname);
    unlink(buf);
    snprintf(buf, sizeof(buf), "%s/sub", dname);
    rmdir(buf);
    rmdir(dname);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_openat")))
long long __imported_wali_openat(int dirfd, const char *pathname, int flags, int mode);

__attribute__((__import_module__("wali"), __import_name__("SYS_mkdirat")))
long long __imported_wali_mkdirat(int dirfd, const char *pathname, int mode);

__attribute__((__import_module__("wali"), __import_name__("SYS_unlinkat")))
long long __imported_wali_unlinkat(int dirfd, const char *pathname, int flags);

int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return (int)__imported_wali_openat(dirfd, pathname, flags, mode); }
int wali_mkdirat(int dirfd, const char *pathname, int mode) { return (int)__imported_wali_mkdirat(dirfd, pathname, mode); }
int wali_unlinkat(int dirfd, const char *pathname, int flags) { return (int)__imported_wali_unlinkat(dirfd, pathname, flags); }

#else
#include <sys/syscall.h>
int wali_openat(int dirfd, const char *pathname, int flags, int mode) { return syscall(SYS_openat, dirfd, pathname, flags, mode); }
int wali_mkdirat(int dirfd, const char *pathname, int mode) { return syscall(SYS_mkdirat, dirfd, pathname, mode); }
int wali_unlinkat(int dirfd, const char *pathname, int flags) { return syscall(SYS_unlinkat, dirfd, pathname, flags); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *op = argv[0];
    const char *dname = argv[1];
    
    int dirfd = open(dname, O_RDONLY | O_DIRECTORY);
    if (dirfd < 0) return -1;
    
    if (strcmp(op, "openat") == 0) {
        // Create f1 inside dname using openat relative
        int fd = wali_openat(dirfd, "f1", O_WRONLY | O_CREAT, 0644);
        if (fd < 0) return -1;
        
        write(fd, "OK", 2);
        close(fd);
        
        // Check exist
        struct stat st;
        char buf[256];
        snprintf(buf, sizeof(buf), "%s/f1", dname);
        if (stat(buf, &st) != 0) return -1;
        
    } else if (strcmp(op, "mkdirat") == 0) {
        if (wali_mkdirat(dirfd, "sub", 0755) != 0) return -1;
        
        struct stat st;
        char buf[256];
        snprintf(buf, sizeof(buf), "%s/sub", dname);
        if (stat(buf, &st) != 0) return -1;
        if (!S_ISDIR(st.st_mode)) return -1;
        
    } else if (strcmp(op, "unlinkat") == 0) {
        // Pre-create file to unlink
        int fd = openat(dirfd, "f1", O_WRONLY | O_CREAT, 0644);
        close(fd);
        
        if (wali_unlinkat(dirfd, "f1", 0) != 0) return -1;
        
        struct stat st;
        char buf[256];
        snprintf(buf, sizeof(buf), "%s/f1", dname);
        if (stat(buf, &st) == 0) return -1; // Should fail
    } 
    
    close(dirfd);
    return 0;
}
