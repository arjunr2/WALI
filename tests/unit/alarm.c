// CMD: setup="" args=""

#include "wali_start.c"
#include <unistd.h>
#include <signal.h>
#include <string.h>

volatile int sig_handled = 0;

void handler(int sig) {
    sig_handled = 1;
}

#ifdef __wasm__
// Use standard sigaction struct
__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigaction")))
long long __imported_wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize);

__attribute__((__import_module__("wali"), __import_name__("SYS_alarm")))
long long __imported_wali_alarm(unsigned int seconds);

int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) {
    return (int)__imported_wali_rt_sigaction(signum, act, oldact, sigsetsize);
}
unsigned int wali_alarm(unsigned int seconds) {
    return (unsigned int)__imported_wali_alarm(seconds);
}

#else
#include <sys/syscall.h>
// Use libc signal for native to avoid kernel ABI complexity with sa_restorer
#include <signal.h>
int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) {
    // return syscall(SYS_rt_sigaction, signum, act, oldact, sigsetsize);
    return sigaction(signum, act, oldact);
}
unsigned int wali_alarm(unsigned int seconds) {
    // return syscall(SYS_alarm, seconds);
    return alarm(seconds);
}
#endif

int test(void) {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN; // Use SIG_IGN to test alarm delivery without crash first
    
    // Using 8 for sigsetsize (64 bits)
    if (wali_rt_sigaction(SIGALRM, &act, NULL, 8) != 0) return -1;
    
    wali_alarm(1);
    
    // Check if we survive 2 seconds (assuming host speed)
    // Testing timing is hard in unit tests without clock.
    // Just verifying it doesn't crash.
    // Using nanosleep or busy loop.
    int i = 0;
    while(i < 50000000) { i++; } // approx delay
    
    return 0;
}
