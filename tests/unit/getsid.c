// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getsid")))
long __imported_wali_getsid(pid_t pid);
__attribute__((__import_module__("wali"), __import_name__("SYS_getpid")))
long __imported_wali_getpid(void);

int wali_getsid(pid_t pid) { return (int)__imported_wali_getsid(pid); }
int wali_getpid(void) { return (int)__imported_wali_getpid(); }

#else
#include <sys/syscall.h>
int wali_getsid(pid_t pid) { return syscall(SYS_getsid, pid); }
int wali_getpid(void) { return syscall(SYS_getpid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Get session ID for current process (pid=0)
    int sid = wali_getsid(0);
    if (sid < 0) return -1;
    
    // Get session ID using explicit pid
    int pid = wali_getpid();
    int sid2 = wali_getsid(pid);
    if (sid2 < 0) return -1;
    
    if (sid != sid2) return -1;
    
    return 0;
}
