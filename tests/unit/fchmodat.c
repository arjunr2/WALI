// CMD: setup="/tmp/fchmodat.txt" args="/tmp/fchmodat.txt" cleanup="/tmp/fchmodat.txt"

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
    // Verify mode is 0640
    struct stat st;
    if (stat(argv[0], &st) != 0) return -1;
    if ((st.st_mode & 0777) != 0640) return -1;

    unlink(argv[0]);
    return 0;
}
#endif

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_fchmodat")))
long __imported_wali_fchmodat(int dirfd, const char *pathname, int mode, int flags);

int wali_fchmodat(int dirfd, const char *pathname, int mode, int flags) { return (int)__imported_wali_fchmodat(dirfd, pathname, mode, flags); }
#else
#include <sys/syscall.h>
int wali_fchmodat(int dirfd, const char *pathname, int mode, int flags) { return syscall(SYS_fchmodat, dirfd, pathname, mode, flags); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *path = argv[1];
    
    // Change using AT_FDCWD
    if (wali_fchmodat(AT_FDCWD, path, 0640, 0) != 0) return -1;
    
    return 0;
}
