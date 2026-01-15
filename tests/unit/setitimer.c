// CMD: args="basic"

#include "wali_start.c"
#include <sys/time.h>
#include <signal.h>
#include <string.h>

static volatile int alarm_count = 0;

static void handler(int sig) {
    alarm_count++;
}

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setitimer")))
long __imported_wali_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigaction")))
long __imported_wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize);

int wali_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) { 
    return (int)__imported_wali_setitimer(which, new_value, old_value); 
}
int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) { 
    return (int)__imported_wali_rt_sigaction(signum, act, oldact, sigsetsize); 
}

#else
#include <sys/syscall.h>
int wali_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) { 
    return syscall(SYS_setitimer, which, new_value, old_value); 
}
int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) { 
    return syscall(SYS_rt_sigaction, signum, act, oldact, sigsetsize); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Setup SIGALRM handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    if (wali_rt_sigaction(SIGALRM, &sa, 0, sizeof(sigset_t)) != 0) return -1;
    
    // Set interval timer
    struct itimerval it;
    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 10000; // 10ms
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    
    if (wali_setitimer(ITIMER_REAL, &it, 0) != 0) return -1;
    
    // Get current timer
    struct itimerval current;
    if (wali_setitimer(ITIMER_REAL, 0, &current) != 0) {
        // Some systems don't allow null new_value, try disabling instead
        it.it_value.tv_sec = 0;
        it.it_value.tv_usec = 0;
        wali_setitimer(ITIMER_REAL, &it, &current);
    }
    
    // Disable timer
    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 0;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    wali_setitimer(ITIMER_REAL, &it, 0);
    
    return 0;
}
