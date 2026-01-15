// CMD: args="basic"
// Note: rt_sigreturn is called implicitly by signal handlers
// We test it indirectly by verifying signal handler returns properly

#include "wali_start.c"
#include <signal.h>
#include <string.h>

static volatile int handler_ran = 0;
static volatile int after_handler = 0;

static void handler(int sig) {
    handler_ran = 1;
    // rt_sigreturn is called implicitly when this returns
}

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigaction")))
long __imported_wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize);
__attribute__((__import_module__("wali"), __import_name__("SYS_kill")))
long __imported_wali_kill(pid_t pid, int sig);
__attribute__((__import_module__("wali"), __import_name__("SYS_getpid")))
long __imported_wali_getpid(void);

int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) { 
    return (int)__imported_wali_rt_sigaction(signum, act, oldact, sigsetsize); 
}
int wali_kill(pid_t pid, int sig) { return (int)__imported_wali_kill(pid, sig); }
pid_t wali_getpid(void) { return (pid_t)__imported_wali_getpid(); }

#else
#include <sys/syscall.h>
int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) { 
    return syscall(SYS_rt_sigaction, signum, act, oldact, sigsetsize); 
}
int wali_kill(pid_t pid, int sig) { return syscall(SYS_kill, pid, sig); }
pid_t wali_getpid(void) { return syscall(SYS_getpid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Setup handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    if (wali_rt_sigaction(SIGUSR1, &sa, 0, sizeof(sigset_t)) != 0) return -1;
    
    // Send signal to self
    if (wali_kill(wali_getpid(), SIGUSR1) != 0) return -1;
    
    // If we get here, rt_sigreturn worked
    after_handler = 1;
    
    if (!handler_ran) return -1;
    if (!after_handler) return -1;
    
    return 0;
}
