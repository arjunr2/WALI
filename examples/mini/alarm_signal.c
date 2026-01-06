#include "common.h"
#include <signal.h>

#define SEC 2

void alarm_signal_handler(int signo) {
  printf("Alarm signal handler: %d\n", signo);
  
  struct sigaction act = {0};
  act.sa_handler = SIG_DFL;
  sigemptyset (&act.sa_mask);
  act.sa_flags = SA_RESTART;

  if (sigaction(SIGALRM, &act, NULL) == -1) {
    printf("Could not install signal in alarm\n");
    exit(1);
  }
  printf("Termininating in 3 seconds\n");
  alarm(3);
}

int main() {
  int off = 0;
  struct sigaction act = {0};
  act.sa_handler = alarm_signal_handler;
  sigemptyset (&act.sa_mask);
  act.sa_flags = SA_RESTART;

  printf("PID: %d\n", getpid());

  if (sigaction(SIGALRM, &act, NULL) == -1) {
    printf("Could not install signal\n");
    exit(1);
  }

  alarm(2);
  int i = 0;
  while (i++ < 100000000) {
    printf("i: %d\n", i);
    sleep(1);
  }
  print("Unreachable\n");

  return 0;
}
