// CMD: setup="" args=""

#include "wali_start.c"
#include <unistd.h>
#include <sys/types.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getgid")))
long long __imported_wali_getgid(void);
gid_t wali_getgid(void) { return (gid_t)__imported_wali_getgid(); }
#else
#include <sys/syscall.h>
gid_t wali_getgid(void) { return syscall(SYS_getgid); }
#endif

int test(void) {
    if (wali_getgid() == (gid_t)-1) return -1;
    return 0;
}
