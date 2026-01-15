// CMD: args="basic"

#include "wali_start.c"
#include <signal.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigprocmask")))
long __imported_wali_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize);

int wali_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize) { 
    return (int)__imported_wali_rt_sigprocmask(how, set, oldset, sigsetsize); 
}

#else
#include <sys/syscall.h>
int wali_rt_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize) { 
    return syscall(SYS_rt_sigprocmask, how, set, oldset, sigsetsize); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    sigset_t set, oldset;
    
    // Get current mask
    if (wali_rt_sigprocmask(SIG_BLOCK, 0, &oldset, sizeof(sigset_t)) != 0) return -1;
    
    // Block SIGUSR1
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    
    if (wali_rt_sigprocmask(SIG_BLOCK, &set, 0, sizeof(sigset_t)) != 0) return -1;
    
    // Verify blocked
    sigset_t current;
    if (wali_rt_sigprocmask(SIG_BLOCK, 0, &current, sizeof(sigset_t)) != 0) return -1;
    
    if (!sigismember(&current, SIGUSR1)) return -1;
    
    // Restore old mask
    if (wali_rt_sigprocmask(SIG_SETMASK, &oldset, 0, sizeof(sigset_t)) != 0) return -1;
    
    return 0;
}
