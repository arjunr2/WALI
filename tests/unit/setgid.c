// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/types.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setgid")))
long __imported_wali_setgid(gid_t gid);
__attribute__((__import_module__("wali"), __import_name__("SYS_getgid")))
long __imported_wali_getgid(void);

int wali_setgid(gid_t gid) { return (int)__imported_wali_setgid(gid); }
gid_t wali_getgid(void) { return (gid_t)__imported_wali_getgid(); }

#else
#include <sys/syscall.h>
int wali_setgid(gid_t gid) { return syscall(SYS_setgid, gid); }
gid_t wali_getgid(void) { return syscall(SYS_getgid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    gid_t g = wali_getgid();
    if (wali_setgid(g) != 0) return -1;
    
    return 0;
}
