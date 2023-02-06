#include "common.h"
#include <signal.h>

#define SEC 2

void custom_signal_handler(int signo) {
  printf("Custom Signal Handler: %s\n", strsignal(signo));
}

int main() {
  int off = 0;
  struct sigaction act = {0};
  act.sa_handler = custom_signal_handler;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;

  printf("PID: %d\n", getpid());

  if (sigaction(SIGINT, &act, NULL) == -1) {
    printf("Could not install signal\n");
    exit(1);
  }
  if (sigaction(SIGHUP, &act, NULL) == -1) {
    printf("Could not install signal\n");
    exit(1);
  }

  sigset_t block_int;
  sigemptyset (&block_int);
  sigaddset (&block_int, SIGINT);
  
  int i = 0;
  while (i < 3) {
    print("Wait for signal (not interrupt)\n");
    i++;
    sigsuspend(&block_int);
    printf("Wake %d\n", i);
  }
  print("Its me\n");

  return 0;
}
