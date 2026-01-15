// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setresuid")))
long __imported_wali_setresuid(uid_t ruid, uid_t euid, uid_t suid);
__attribute__((__import_module__("wali"), __import_name__("SYS_getuid")))
long __imported_wali_getuid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_geteuid")))
long __imported_wali_geteuid(void);

int wali_setresuid(uid_t ruid, uid_t euid, uid_t suid) { return (int)__imported_wali_setresuid(ruid, euid, suid); }
uid_t wali_getuid(void) { return (uid_t)__imported_wali_getuid(); }
uid_t wali_geteuid(void) { return (uid_t)__imported_wali_geteuid(); }

#else
#include <sys/syscall.h>
int wali_setresuid(uid_t ruid, uid_t euid, uid_t suid) { return syscall(SYS_setresuid, ruid, euid, suid); }
uid_t wali_getuid(void) { return syscall(SYS_getuid); }
uid_t wali_geteuid(void) { return syscall(SYS_geteuid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    uid_t ruid = wali_getuid();
    uid_t euid = wali_geteuid();
    
    // Setting to same values should always succeed
    int ret = wali_setresuid(ruid, euid, euid);
    if (ret != 0) return -1;
    
    // Verify unchanged
    if (wali_getuid() != ruid) return -1;
    if (wali_geteuid() != euid) return -1;
    
    return 0;
}
