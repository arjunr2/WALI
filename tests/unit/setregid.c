// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/types.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setregid")))
long __imported_wali_setregid(gid_t rgid, gid_t egid);

int wali_setregid(gid_t rgid, gid_t egid) { return (int)__imported_wali_setregid(rgid, egid); }

#else
#include <sys/syscall.h>
int wali_setregid(gid_t rgid, gid_t egid) { return syscall(SYS_setregid, rgid, egid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // -1 means no change
    if (wali_setregid(-1, -1) != 0) return -1;
    
    return 0;
}
