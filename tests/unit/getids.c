// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/types.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getuid")))
long long __imported_wali_getuid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_getgid")))
long long __imported_wali_getgid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_geteuid")))
long long __imported_wali_geteuid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_getegid")))
long long __imported_wali_getegid(void);

uid_t wali_getuid(void) { return (uid_t)__imported_wali_getuid(); }
gid_t wali_getgid(void) { return (gid_t)__imported_wali_getgid(); }
uid_t wali_geteuid(void) { return (uid_t)__imported_wali_geteuid(); }
gid_t wali_getegid(void) { return (gid_t)__imported_wali_getegid(); }

#else
#include <sys/syscall.h>
uid_t wali_getuid(void) { return syscall(SYS_getuid); }
gid_t wali_getgid(void) { return syscall(SYS_getgid); }
uid_t wali_geteuid(void) { return syscall(SYS_geteuid); }
gid_t wali_getegid(void) { return syscall(SYS_getegid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    if (wali_getuid() == (uid_t)-1) return -1;
    if (wali_getgid() == (gid_t)-1) return -1;
    if (wali_geteuid() == (uid_t)-1) return -1;
    if (wali_getegid() == (gid_t)-1) return -1;
    
    return 0;
}
