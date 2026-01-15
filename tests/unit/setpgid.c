// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setpgid")))
long __imported_wali_setpgid(pid_t pid, pid_t pgid);
__attribute__((__import_module__("wali"), __import_name__("SYS_getpgid")))
long __imported_wali_getpgid(pid_t pid);

int wali_setpgid(pid_t pid, pid_t pgid) { return (int)__imported_wali_setpgid(pid, pgid); }
int wali_getpgid(pid_t pid) { return (int)__imported_wali_getpgid(pid); }

#else
#include <sys/syscall.h>
int wali_setpgid(pid_t pid, pid_t pgid) { return syscall(SYS_setpgid, pid, pgid); }
int wali_getpgid(pid_t pid) { return syscall(SYS_getpgid, pid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Get current pgid
    int pgid = wali_getpgid(0);
    if (pgid < 0) return -1;
    
    // Set pgid to same value (should succeed)
    if (wali_setpgid(0, pgid) != 0) return -1;
    
    // Verify still the same
    int pgid2 = wali_getpgid(0);
    if (pgid2 != pgid) return -1;
    
    return 0;
}
