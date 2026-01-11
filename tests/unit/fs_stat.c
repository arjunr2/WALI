// CMD: setup="" args=""

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/statfs.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_statfs")))
long long __imported_wali_statfs(const char *path, struct statfs *buf);
__attribute__((__import_module__("wali"), __import_name__("SYS_fstatfs")))
long long __imported_wali_fstatfs(int fd, struct statfs *buf);

int wali_statfs(const char *path, struct statfs *buf) { return (int)__imported_wali_statfs(path, buf); }
int wali_fstatfs(int fd, struct statfs *buf) { return (int)__imported_wali_fstatfs(fd, buf); }
#else
#include <sys/syscall.h>
int wali_statfs(const char *path, struct statfs *buf) { return syscall(SYS_statfs, path, buf); }
int wali_fstatfs(int fd, struct statfs *buf) { return syscall(SYS_fstatfs, fd, buf); }
#endif

int test(void) {
    struct statfs st;
    if (wali_statfs(".", &st) != 0) return -1;
    
    // Sanity check
    if (st.f_type == 0) return -1;
    
    int fd = open(".", O_RDONLY);
    if (fd < 0) return -1;
    
    struct statfs st2;
    if (wali_fstatfs(fd, &st2) != 0) return -1;
    if (st2.f_type != st.f_type) return -1;
    
    close(fd);
    return 0;
}
