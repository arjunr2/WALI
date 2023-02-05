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

  act.sa_handler = SIG_DFL;
  if (sigaction(SIGINT, &act, NULL) == -1) {
    printf("Could not install signal\n");
    exit(1);
  }


  int i = 0;
  while (i++ < 100000000) {
    print("Wait 2 sec\n");
    sleep(2);
  }
  print("Its me\n");

  return 0;
}
