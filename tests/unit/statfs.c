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

int wali_statfs(const char *path, struct statfs *buf) { return (int)__imported_wali_statfs(path, buf); }
#else
#include <sys/syscall.h>
int wali_statfs(const char *path, struct statfs *buf) { return syscall(SYS_statfs, path, buf); }
#endif

int test(void) {
    struct statfs st;
    if (wali_statfs(".", &st) != 0) return -1;
    
    // Sanity check
    if (st.f_type == 0) return -1;
    
    return 0;
}
