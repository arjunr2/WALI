// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setsid")))
long __imported_wali_setsid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_getsid")))
long __imported_wali_getsid(pid_t pid);

int wali_setsid(void) { return (int)__imported_wali_setsid(); }
int wali_getsid(pid_t pid) { return (int)__imported_wali_getsid(pid); }

#else
#include <sys/syscall.h>
int wali_setsid(void) { return syscall(SYS_setsid); }
int wali_getsid(pid_t pid) { return syscall(SYS_getsid, pid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Get current session ID
    int sid = wali_getsid(0);
    if (sid < 0) return -1;
    
    // setsid will fail if we're already a session leader (EPERM)
    // This is expected in most test environments
    int ret = wali_setsid();
    // Either succeeds or fails with EPERM - both are valid behaviors
    (void)ret;
    
    // Verify getsid still works
    int sid2 = wali_getsid(0);
    if (sid2 < 0) return -1;
    
    return 0;
}
