// CMD: setup="create /tmp/fchownat_test" args="/tmp/fchownat_test" cleanup="remove /tmp/fchownat_test"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return 0;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
__attribute__((__import_module__("wali"), __import_name__("SYS_fchownat")))
long __imported_wali_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_getuid")))
long __imported_wali_getuid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_getgid")))
long __imported_wali_getgid(void);

int wali_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) { 
    return (int)__imported_wali_fchownat(dirfd, pathname, owner, group, flags); 
}
uid_t wali_getuid(void) { return (uid_t)__imported_wali_getuid(); }
gid_t wali_getgid(void) { return (gid_t)__imported_wali_getgid(); }

#else
#include <sys/syscall.h>
int wali_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) { 
    return syscall(SYS_fchownat, dirfd, pathname, owner, group, flags); 
}
uid_t wali_getuid(void) { return syscall(SYS_getuid); }
gid_t wali_getgid(void) { return syscall(SYS_getgid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *path = argv[0];
    
    uid_t uid = wali_getuid();
    gid_t gid = wali_getgid();
    
    // Set to current owner/group using AT_FDCWD
    int ret = wali_fchownat(AT_FDCWD, path, uid, gid, 0);
    if (ret != 0) return -1;
    
    return 0;
}
