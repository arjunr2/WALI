// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <grp.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getgroups")))
long __imported_wali_getgroups(int size, gid_t *list);

int wali_getgroups(int size, gid_t *list) { return (int)__imported_wali_getgroups(size, list); }

#else
#include <sys/syscall.h>
int wali_getgroups(int size, gid_t *list) { return syscall(SYS_getgroups, size, list); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Get number of groups
    int ngroups = wali_getgroups(0, 0);
    if (ngroups < 0) return -1;
    
    // Get actual groups if any
    if (ngroups > 0) {
        gid_t groups[64];
        int n = wali_getgroups(64, groups);
        if (n != ngroups) return -1;
    }
    
    return 0;
}
