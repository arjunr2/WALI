// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_exit")))
long __imported_wali_exit(int status);
__attribute__((__import_module__("wali"), __import_name__("SYS_fork")))
long __imported_wali_fork(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_wait4")))
long __imported_wali_wait4(int pid, int *wstatus, int options, void *rusage);

void wali_exit(int status) { __imported_wali_exit(status); }
int wali_fork(void) { return (int)__imported_wali_fork(); }
int wali_wait4(int pid, int *wstatus, int options, void *rusage) { 
    return (int)__imported_wali_wait4(pid, wstatus, options, rusage); 
}

#else
#include <sys/syscall.h>
#include <sys/wait.h>
void wali_exit(int status) { syscall(SYS_exit, status); }
int wali_fork(void) { return syscall(SYS_fork); }
int wali_wait4(int pid, int *wstatus, int options, void *rusage) { 
    return syscall(SYS_wait4, pid, wstatus, options, rusage); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Fork and have child call exit
    int pid = wali_fork();
    if (pid < 0) return -1;
    
    if (pid == 0) {
        // Child exits with status 7
        wali_exit(7);
    }
    
    // Parent waits
    int status = 0;
    int ret = wali_wait4(pid, &status, 0, 0);
    if (ret != pid) return -1;
    
    // Verify exit status
    if (!WIFEXITED(status)) return -1;
    if (WEXITSTATUS(status) != 7) return -1;
    
    return 0;
}
