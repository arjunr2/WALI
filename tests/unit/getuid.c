// CMD: setup="" args=""

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
uid_t wali_getuid(void) { return (uid_t)__imported_wali_getuid(); }
#else
#include <sys/syscall.h>
uid_t wali_getuid(void) { return syscall(SYS_getuid); }
#endif

int test(void) {
    if (wali_getuid() == (uid_t)-1) return -1;
    return 0;
}
