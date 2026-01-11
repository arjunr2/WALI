// CMD: args="ignore"

#include "wali_start.c"
#include <unistd.h>
#include <signal.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    return 0;
}

int test_cleanup(int argc, char **argv) {
    return 0;
}
#endif

#ifdef __wasm__
// Structs for sigaction are tricky between archs.
// WALI defines its own structs in wali.wit which map to Linux x86_64 usually?
// But we should use the headers provided in the sysroot if valid.
// Assuming musl sysroot has correct defs.

__attribute__((__import_module__("wali"), __import_name__("SYS_rt_sigaction")))
long long __imported_wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize);

__attribute__((__import_module__("wali"), __import_name__("SYS_kill")))
long long __imported_wali_kill(pid_t pid, int sig);

__attribute__((__import_module__("wali"), __import_name__("SYS_getpid")))
long long __imported_wali_getpid(void);

int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) {
    return (int)__imported_wali_rt_sigaction(signum, act, oldact, sigsetsize);
}
int wali_kill(pid_t pid, int sig) {
    return (int)__imported_wali_kill(pid, sig);
}
pid_t wali_getpid(void) {
    return (pid_t)__imported_wali_getpid();
}

#else
#include <sys/syscall.h>
int wali_rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, size_t sigsetsize) {
    return syscall(SYS_rt_sigaction, signum, act, oldact, sigsetsize);
}
int wali_kill(pid_t pid, int sig) {
    return syscall(SYS_kill, pid, sig);
}
pid_t wali_getpid(void) {
    return syscall(SYS_getpid);
}
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *mode = (argc > 0) ? argv[0] : "fail";
  
  if (strcmp(mode, "ignore") == 0) {
      struct sigaction sa;
      memset(&sa, 0, sizeof(sa));
      sa.sa_handler = SIG_IGN;
      
      // SIGUSR1 = 10 usually
      if (wali_rt_sigaction(SIGUSR1, &sa, NULL, sizeof(sigset_t)) != 0) return -1;
      
      pid_t mypid = wali_getpid();
      if (mypid <= 0) return -1;
      
      // Send signal. Process should NOT die.
      if (wali_kill(mypid, SIGUSR1) != 0) return -1;
      
      // If we are here, we survived.
      return 0;
  }
  
  return -1;
}
