#include "common.h"
#include <signal.h>

#define SEC 2

void custom_signal_handler(int signo) {
  //printf("Custom Signal Handler: %s\n", strsignal(signo));
  printf("Custom Signal Handler: %d\n", signo);
}

int main() {
  int off = 0;
  struct sigaction act = {0};
  act.sa_handler = custom_signal_handler;
  sigemptyset (&act.sa_mask);
  act.sa_flags = SA_RESTART;

  printf("PID: %d\n", getpid());

  if (sigaction(SIGINT, &act, NULL) == -1) {
    printf("Could not install signal\n");
    exit(1);
  }

  int i = 0;
  while (i++ < 100000000) {
    print("Wait 2 sec\n");
    sleep(2);
  }
  print("Unreachable\n");

  return 0;
}
