// CMD: args="basic"

#include "wali_start.c"
#include <signal.h>
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_tkill")))
long __imported_wali_tkill(int tid, int sig);
__attribute__((__import_module__("wali"), __import_name__("SYS_gettid")))
long __imported_wali_gettid(void);

int wali_tkill(int tid, int sig) { return (int)__imported_wali_tkill(tid, sig); }
int wali_gettid(void) { return (int)__imported_wali_gettid(); }

#else
#include <sys/syscall.h>
int wali_tkill(int tid, int sig) { return syscall(SYS_tkill, tid, sig); }
int wali_gettid(void) { return syscall(SYS_gettid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int tid = wali_gettid();
    if (tid <= 0) return -1;
    
    // Send signal 0 (existence check)
    if (wali_tkill(tid, 0) != 0) return -1;
    
    return 0;
}
