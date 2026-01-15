// CMD: args="basic"

#include "wali_start.c"
#include <signal.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigpending")))
long __imported_wali_rt_sigpending(sigset_t *set, size_t sigsetsize);
__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigprocmask")))
long __imported_wali_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize);
__attribute__((__import_module__("wali"), __import_name__("SYS_kill")))
long __imported_wali_kill(pid_t pid, int sig);
__attribute__((__import_module__("wali"), __import_name__("SYS_getpid")))
long __imported_wali_getpid(void);

int wali_rt_sigpending(sigset_t *set, size_t sigsetsize) { 
    return (int)__imported_wali_rt_sigpending(set, sigsetsize); 
}
int wali_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize) { 
    return (int)__imported_wali_rt_sigprocmask(how, set, oldset, sigsetsize); 
}
int wali_kill(pid_t pid, int sig) { return (int)__imported_wali_kill(pid, sig); }
pid_t wali_getpid(void) { return (pid_t)__imported_wali_getpid(); }

#else
#include <sys/syscall.h>
int wali_rt_sigpending(sigset_t *set, size_t sigsetsize) { 
    return syscall(SYS_rt_sigpending, set, sigsetsize); 
}
int wali_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize) { 
    return syscall(SYS_rt_sigprocmask, how, set, oldset, sigsetsize); 
}
int wali_kill(pid_t pid, int sig) { return syscall(SYS_kill, pid, sig); }
pid_t wali_getpid(void) { return syscall(SYS_getpid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    sigset_t oldmask;
    
    // Block SIGUSR1
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    if (wali_rt_sigprocmask(SIG_BLOCK, &block, &oldmask, sizeof(sigset_t)) != 0) return -1;
    
    // Send SIGUSR1 to self (will be pending)
    if (wali_kill(wali_getpid(), SIGUSR1) != 0) {
        wali_rt_sigprocmask(SIG_SETMASK, &oldmask, 0, sizeof(sigset_t));
        return -1;
    }
    
    // Check pending
    sigset_t pending;
    if (wali_rt_sigpending(&pending, sizeof(sigset_t)) != 0) {
        wali_rt_sigprocmask(SIG_SETMASK, &oldmask, 0, sizeof(sigset_t));
        return -1;
    }
    
    int is_pending = sigismember(&pending, SIGUSR1);
    
    // Ignore the signal before unblocking
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGUSR1, &sa, 0);
    
    // Restore mask
    wali_rt_sigprocmask(SIG_SETMASK, &oldmask, 0, sizeof(sigset_t));
    
    if (!is_pending) return -1;
    
    return 0;
}
