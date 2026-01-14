// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_kill")))
long __imported_wali_kill(int pid, int sig);
__attribute__((__import_module__("wali"), __import_name__("SYS_tkill")))
long __imported_wali_tkill(int tid, int sig);
__attribute__((__import_module__("wali"), __import_name__("SYS_getpid")))
long __imported_wali_getpid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_gettid")))
long __imported_wali_gettid(void);

int wali_kill(int pid, int sig) { return (int)__imported_wali_kill(pid, sig); }
int wali_tkill(int tid, int sig) { return (int)__imported_wali_tkill(tid, sig); }
int wali_getpid(void) { return (int)__imported_wali_getpid(); }
int wali_gettid(void) { return (int)__imported_wali_gettid(); }

#else
#include <sys/syscall.h>
int wali_kill(int pid, int sig) { return syscall(SYS_kill, pid, sig); }
int wali_tkill(int tid, int sig) { return syscall(SYS_tkill, tid, sig); }
pid_t wali_getpid(void) { return syscall(SYS_getpid); }
pid_t wali_gettid(void) { return syscall(SYS_gettid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int pid = wali_getpid();
    // Test 0 signal (existence check)
    if (wali_kill(pid, 0) != 0) return -1;
    
    // Test tkill
    int tid = wali_gettid();
    if (wali_tkill(tid, 0) != 0) return -1;
    
    return 0;
}
