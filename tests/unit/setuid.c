// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setuid")))
long __imported_wali_setuid(uid_t uid);
__attribute__((__import_module__("wali"), __import_name__("SYS_getuid")))
long __imported_wali_getuid(void);

int wali_setuid(uid_t uid) { return (int)__imported_wali_setuid(uid); }
uid_t wali_getuid(void) { return (uid_t)__imported_wali_getuid(); }

#else
#include <sys/syscall.h>
int wali_setuid(uid_t uid) { return syscall(SYS_setuid, uid); }
uid_t wali_getuid(void) { return syscall(SYS_getuid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    uid_t u = wali_getuid();
    // Setting to current uid should succeed
    if (wali_setuid(u) != 0) return -1;
    
    // Setting to something else (e.g. 0) might fail if not root
    // We don't enforce failure/success here as it depends on runner privileges,
    // but we tested the syscall path.
    
    return 0;
}
