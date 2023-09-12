#include "common.h"
#include <signal.h>

int global_val = 4;

void custom_signal_handler(int signo) {
  //printf("Custom Signal Handler: %s\n", strsignal(signo));
  printf("Custom Signal Handler: %d\n", signo);
}

int main() {
  int off = 0;
  struct sigaction act = {0};
  act.sa_handler = custom_signal_handler;
  sigemptyset (&act.sa_mask);

  printf("PID: %d\n", getpid());

  if (sigaction(SIGINT, &act, NULL) == -1) {
    printf("Could not install signal\n");
    exit(1);
  }

  int i = 0;
  /* Infinite futex wait */
  syscall(SYS_futex, &global_val, 0, 4, NULL, 0, 0);
  printf("Finished end!\n");

  return 0;
}
