// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

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
__attribute__((__import_module__("wali"), __import_name__("SYS_fork")))
long long __imported_wali_fork(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_wait4")))
long long __imported_wali_wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage);
__attribute__((__import_module__("wali"), __import_name__("SYS_exit")))
long long __imported_wali_exit(int status);

pid_t wali_fork(void) { return (pid_t)__imported_wali_fork(); }
pid_t wali_wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage) { return (pid_t)__imported_wali_wait4(pid, wstatus, options, rusage); }
void wali_exit(int status) { __imported_wali_exit(status); while(1); }

#else
#include <sys/syscall.h>
#include <sys/resource.h>
pid_t wali_fork(void) { return syscall(SYS_fork); }
pid_t wali_wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage) { return syscall(SYS_wait4, pid, wstatus, options, rusage); }
void wali_exit(int status) { syscall(SYS_exit, status); } 
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *mode = (argc > 0) ? argv[0] : "fail";
  
  if (strcmp(mode, "basic") == 0) {
      pid_t pid = wali_fork();
      if (pid < 0) return -1;
      
      if (pid == 0) {
          // Child
          wali_exit(42);
          return -1; // Should not reach
      } else {
          // Parent
          int status = 0;
          pid_t res = wali_wait4(pid, &status, 0, NULL);
          if (res != pid) return -1;
          
          if (!WIFEXITED(status)) return -1;
          if (WEXITSTATUS(status) != 42) return -1;
          
          return 0;
      }
  }
  
  return -1;
}
