// CMD: args="basic"

#include "wali_start.c"
#include <signal.h>
#include <string.h>
#include <unistd.h>

static volatile int got_signal = 0;

static void handler(int sig) {
    got_signal = 1;
}

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigsuspend")))
long __imported_wali_rt_sigsuspend(const sigset_t *mask, size_t sigsetsize);
__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigprocmask")))
long __imported_wali_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize);
__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigaction")))
long __imported_wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize);
__attribute__((__import_module__("wali"), __import_name__("SYS_fork")))
long __imported_wali_fork(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_exit")))
long __imported_wali_exit(int status);
__attribute__((__import_module__("wali"), __import_name__("SYS_kill")))
long __imported_wali_kill(pid_t pid, int sig);
__attribute__((__import_module__("wali"), __import_name__("SYS_getppid")))
long __imported_wali_getppid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_nanosleep")))
long __imported_wali_nanosleep(const struct timespec *req, struct timespec *rem);
__attribute__((__import_module__("wali"), __import_name__("SYS_wait4")))
long __imported_wali_wait4(pid_t pid, int *status, int options, void *rusage);

int wali_rt_sigsuspend(const sigset_t *mask, size_t sigsetsize) { 
    return (int)__imported_wali_rt_sigsuspend(mask, sigsetsize); 
}
int wali_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize) { 
    return (int)__imported_wali_rt_sigprocmask(how, set, oldset, sigsetsize); 
}
int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) { 
    return (int)__imported_wali_rt_sigaction(signum, act, oldact, sigsetsize); 
}
pid_t wali_fork(void) { return (pid_t)__imported_wali_fork(); }
void wali_exit(int status) { __imported_wali_exit(status); }
int wali_kill(pid_t pid, int sig) { return (int)__imported_wali_kill(pid, sig); }
pid_t wali_getppid(void) { return (pid_t)__imported_wali_getppid(); }
int wali_nanosleep(const struct timespec *req, struct timespec *rem) { return (int)__imported_wali_nanosleep(req, rem); }
int wali_wait4(pid_t pid, int *status, int options, void *rusage) { return (int)__imported_wali_wait4(pid, status, options, rusage); }

#else
#include <sys/syscall.h>
#include <sys/wait.h>
int wali_rt_sigsuspend(const sigset_t *mask, size_t sigsetsize) { 
    return syscall(SYS_rt_sigsuspend, mask, sigsetsize); 
}
int wali_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize) { 
    return syscall(SYS_rt_sigprocmask, how, set, oldset, sigsetsize); 
}
int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) { 
    return syscall(SYS_rt_sigaction, signum, act, oldact, sigsetsize); 
}
pid_t wali_fork(void) { return syscall(SYS_fork); }
void wali_exit(int status) { syscall(SYS_exit, status); }
int wali_kill(pid_t pid, int sig) { return syscall(SYS_kill, pid, sig); }
pid_t wali_getppid(void) { return syscall(SYS_getppid); }
int wali_nanosleep(const struct timespec *req, struct timespec *rem) { return syscall(SYS_nanosleep, req, rem); }
int wali_wait4(pid_t pid, int *status, int options, void *rusage) { return syscall(SYS_wait4, pid, status, options, rusage); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Setup handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    if (wali_rt_sigaction(SIGUSR1, &sa, 0, sizeof(sigset_t)) != 0) return -1;
    
    // Block SIGUSR1
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    if (wali_rt_sigprocmask(SIG_BLOCK, &block, 0, sizeof(sigset_t)) != 0) return -1;
    
    pid_t pid = wali_fork();
    if (pid < 0) return -1;
    
    if (pid == 0) {
        // Child: wait then signal parent
        struct timespec ts = { .tv_sec = 0, .tv_nsec = 50000000 };
        wali_nanosleep(&ts, 0);
        wali_kill(wali_getppid(), SIGUSR1);
        wali_exit(0);
    }
    
    // Parent: suspend with empty mask (SIGUSR1 unblocked)
    sigset_t empty;
    sigemptyset(&empty);
    wali_rt_sigsuspend(&empty, sizeof(sigset_t));
    
    int status;
    wali_wait4(pid, &status, 0, 0);
    
    if (!got_signal) return -1;
    
    return 0;
}
