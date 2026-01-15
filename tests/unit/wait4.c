// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_wait4")))
long __imported_wali_wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage);
__attribute__((__import_module__("wali"), __import_name__("SYS_fork")))
long __imported_wali_fork(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_exit")))
long __imported_wali_exit(int status);

pid_t wali_wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage) { 
    return (pid_t)__imported_wali_wait4(pid, wstatus, options, rusage); 
}
pid_t wali_fork(void) { return (pid_t)__imported_wali_fork(); }
void wali_exit(int status) { __imported_wali_exit(status); }

#else
#include <sys/syscall.h>
#include <sys/resource.h>
pid_t wali_wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage) { 
    return syscall(SYS_wait4, pid, wstatus, options, rusage); 
}
pid_t wali_fork(void) { return syscall(SYS_fork); }
void wali_exit(int status) { syscall(SYS_exit, status); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    pid_t pid = wali_fork();
    if (pid < 0) return -1;
    
    if (pid == 0) {
        // Child exits with 42
        wali_exit(42);
    }
    
    // Parent waits
    int status = 0;
    pid_t ret = wali_wait4(pid, &status, 0, 0);
    if (ret != pid) return -1;
    
    // Check child exited with 42
    if (!WIFEXITED(status)) return -1;
    if (WEXITSTATUS(status) != 42) return -1;
    
    return 0;
}
