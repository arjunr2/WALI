// CMD: setup="create /tmp/chown_test" args="file /tmp/chown_test"
// CMD: setup="create /tmp/chown_fd" args="fd /tmp/chown_fd"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_chown")))
long __imported_wali_chown(const char *pathname, int owner, int group);
__attribute__((__import_module__("wali"), __import_name__("SYS_fchown")))
long __imported_wali_fchown(int fd, int owner, int group);
__attribute__((__import_module__("wali"), __import_name__("SYS_fchownat")))
long __imported_wali_fchownat(int dirfd, const char *pathname, int owner, int group, int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_chown(const char *pathname, int owner, int group) { return (int)__imported_wali_chown(pathname, owner, group); }
int wali_fchown(int fd, int owner, int group) { return (int)__imported_wali_fchown(fd, owner, group); }
int wali_fchownat(int dirfd, const char *pathname, int owner, int group, int flags) { return (int)__imported_wali_fchownat(dirfd, pathname, owner, group, flags); }
int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_chown(const char *pathname, int owner, int group) { return syscall(SYS_chown, pathname, owner, group); }
int wali_fchown(int fd, int owner, int group) { return syscall(SYS_fchown, fd, owner, group); }
int wali_fchownat(int dirfd, const char *pathname, int owner, int group, int flags) { return syscall(SYS_fchownat, dirfd, pathname, owner, group, flags); }
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    if (argc < 2) return -1;
    
    const char *mode = argv[0];
    const char *path = argv[1];
    
    // We can't really change ownership to arbitrary users without root.
    // typically -1 means "don't change".
    // Or we can try to change to current uid/gid, which should succeed.
    
    // Getting current uid/gid is tricky without getuid/getgid import in this file.
    // But -1 should be a safe no-op that returns success.
    
    if (strcmp(mode, "file") == 0) {
        if (wali_chown(path, -1, -1) != 0) return -1;
        
        // fchownat
        if (wali_fchownat(AT_FDCWD, path, -1, -1, 0) != 0) return -1;
        
    } else if (strcmp(mode, "fd") == 0) {
        int fd = wali_open(path, O_RDONLY, 0);
        if (fd < 0) return -1;
        
        if (wali_fchown(fd, -1, -1) != 0) {
            wali_close(fd);
            return -1;
        }
        wali_close(fd);
    }
    
    return 0;
}
