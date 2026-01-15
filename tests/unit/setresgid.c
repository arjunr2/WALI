// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setresgid")))
long __imported_wali_setresgid(gid_t rgid, gid_t egid, gid_t sgid);
__attribute__((__import_module__("wali"), __import_name__("SYS_getgid")))
long __imported_wali_getgid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_getegid")))
long __imported_wali_getegid(void);

int wali_setresgid(gid_t rgid, gid_t egid, gid_t sgid) { return (int)__imported_wali_setresgid(rgid, egid, sgid); }
gid_t wali_getgid(void) { return (gid_t)__imported_wali_getgid(); }
gid_t wali_getegid(void) { return (gid_t)__imported_wali_getegid(); }

#else
#include <sys/syscall.h>
int wali_setresgid(gid_t rgid, gid_t egid, gid_t sgid) { return syscall(SYS_setresgid, rgid, egid, sgid); }
gid_t wali_getgid(void) { return syscall(SYS_getgid); }
gid_t wali_getegid(void) { return syscall(SYS_getegid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    gid_t rgid = wali_getgid();
    gid_t egid = wali_getegid();
    
    // Setting to same values should always succeed
    int ret = wali_setresgid(rgid, egid, egid);
    if (ret != 0) return -1;
    
    // Verify unchanged
    if (wali_getgid() != rgid) return -1;
    if (wali_getegid() != egid) return -1;
    
    return 0;
}
