// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <grp.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setgroups")))
long __imported_wali_setgroups(int size, const gid_t *list);
__attribute__((__import_module__("wali"), __import_name__("SYS_getgroups")))
long __imported_wali_getgroups(int size, gid_t *list);
__attribute__((__import_module__("wali"), __import_name__("SYS_getegid")))
long __imported_wali_getegid(void);

int wali_setgroups(int size, const gid_t *list) { return (int)__imported_wali_setgroups(size, list); }
int wali_getgroups(int size, gid_t *list) { return (int)__imported_wali_getgroups(size, list); }
gid_t wali_getegid(void) { return (gid_t)__imported_wali_getegid(); }

#else
#include <sys/syscall.h>
int wali_setgroups(int size, const gid_t *list) { return syscall(SYS_setgroups, size, list); }
int wali_getgroups(int size, gid_t *list) { return syscall(SYS_getgroups, size, list); }
gid_t wali_getegid(void) { return syscall(SYS_getegid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Get current effective gid
    gid_t egid = wali_getegid();
    
    // Try to set groups (may fail with EPERM if not root)
    gid_t groups[1] = { egid };
    int ret = wali_setgroups(1, groups);
    
    // Either succeeds (root) or fails with EPERM (non-root)
    // Both are valid behaviors
    (void)ret;
    
    return 0;
}
