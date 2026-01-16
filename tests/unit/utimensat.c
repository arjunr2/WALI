// CMD: setup="/tmp/time_file" args="utimensat /tmp/time_file" cleanup="/tmp/time_file"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    int fd = open(argv[0], O_WRONLY | O_CREAT, 0644);
    close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    unlink(argv[0]);
    return 0;
}
#endif

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_utimensat")))
long long __imported_wali_utimensat(int dirfd, const char *pathname, const struct timespec *times, int flags);
int wali_utimensat(int dirfd, const char *pathname, const struct timespec *times, int flags) { 
    return (int)__imported_wali_utimensat(dirfd, pathname, times, flags); 
}
#else
#include <sys/syscall.h>
int wali_utimensat(int dirfd, const char *pathname, const struct timespec *times, int flags) { 
    return syscall(SYS_utimensat, dirfd, pathname, times, flags); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    
    struct timespec ts[2];
    // Access time
    ts[0].tv_sec = 1000;
    ts[0].tv_nsec = 0;
    // Mod time
    ts[1].tv_sec = 2000;
    ts[1].tv_nsec = 0;
    
    if (wali_utimensat(AT_FDCWD, fname, ts, 0) != 0) return -1;
    
    struct stat st;
    if (stat(fname, &st) != 0) return -1;
    
    if (st.st_atime != 1000) return -1;
    if (st.st_mtime != 2000) return -1;
    
    return 0;
}
