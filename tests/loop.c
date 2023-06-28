#include "common.h"

void custom_signal_handler(int signo) {
  printf("Custom Signal Handler: %s\n", strsignal(signo));
}

int main(int argc, char *argv[]) {
  long long N = 0;
  if (argc > 1) {
    N = atoi(argv[1]);
  }

  struct sigaction act = {0};
  act.sa_handler = custom_signal_handler;
  sigemptyset (&act.sa_mask);
  act.sa_flags = SA_RESTART;

  printf("PID: %d\n", getpid());

  if (sigaction(SIGINT, &act, NULL) == -1) {
    printf("Could not install signal\n");
    exit(1);
  }

  volatile long long i = 0;
  while (i < N) {
    if ((i % 10000000) == 0) {
      printf("Ct 10m\n");
    }
    i++;
  }
}
