// CMD: setup="/tmp/newfstatat_test" args="/tmp/newfstatat_test" cleanup="remove /tmp/newfstatat_test"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return 0;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, "DATA", 4);
        close(fd);
    }
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return 0;
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_newfstatat")))
long __imported_wali_newfstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags);

int wali_newfstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) { 
    return (int)__imported_wali_newfstatat(dirfd, pathname, statbuf, flags); 
}

#else
#include <sys/syscall.h>
int wali_newfstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) { 
    return syscall(SYS_newfstatat, dirfd, pathname, statbuf, flags); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *path = argv[0];
    
    struct stat st;
    
    // Use AT_FDCWD with absolute path
    if (wali_newfstatat(AT_FDCWD, path, &st, 0) != 0) return -1;
    
    if (!S_ISREG(st.st_mode)) return -1;
    if (st.st_size != 4) return -1;
    
    return 0;
}
