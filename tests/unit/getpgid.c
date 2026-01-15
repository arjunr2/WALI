// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getpgid")))
long __imported_wali_getpgid(pid_t pid);
__attribute__((__import_module__("wali"), __import_name__("SYS_getpid")))
long __imported_wali_getpid(void);

int wali_getpgid(pid_t pid) { return (int)__imported_wali_getpgid(pid); }
int wali_getpid(void) { return (int)__imported_wali_getpid(); }

#else
#include <sys/syscall.h>
int wali_getpgid(pid_t pid) { return syscall(SYS_getpgid, pid); }
int wali_getpid(void) { return syscall(SYS_getpid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Get process group ID for current process (pid=0)
    int pgid = wali_getpgid(0);
    if (pgid < 0) return -1;
    
    // Get process group ID using explicit pid
    int pid = wali_getpid();
    int pgid2 = wali_getpgid(pid);
    if (pgid2 < 0) return -1;
    
    if (pgid != pgid2) return -1;
    
    return 0;
}
