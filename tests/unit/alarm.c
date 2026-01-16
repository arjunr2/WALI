// CMD: setup="" args="" cleanup=""

#include "wali_start.c"

#ifdef __wasm__
WALI_IMPORT("SYS_alarm") long wali_syscall_alarm(unsigned int seconds);
#else
#include <sys/syscall.h>
long wali_syscall_alarm(unsigned int seconds) { return syscall(SYS_alarm, seconds); }
#endif
#include <signal.h>

volatile int sig_handled = 0;

int test(void) {
    struct sigaction act;
    char *p = (char*)&act;
    for(int i=0; i<sizeof(act); i++) p[i] = 0;
    
    act.sa_handler = SIG_IGN; 
    
    // 1 second alarm
    TEST_ASSERT_EQ(wali_syscall_rt_sigaction(SIGALRM, &act, NULL, 8), 0);
    
    wali_syscall_alarm(1);
    
    // Busy wait approx 2 sec
    // 50M iters might be too fast or too slow depending on CPU.
    // But since we ignore the signal, we just want to ensure we don't crash.
    volatile int i = 0;
    while(i < 50000000) { i++; }
    
    return 0;
}
