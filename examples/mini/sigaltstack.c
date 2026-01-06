#include "common.h"

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

  stack_t ss;
  ss.ss_sp = malloc(SIGSTKSZ);
  if (!ss.ss_sp) {
    printf("Malloc error\n");
    exit(1);
  }
  ss.ss_size = SIGSTKSZ;
  ss.ss_flags = 0;
  if (sigaltstack(&ss, NULL) == -1) {
    printf("Sigaltstack error\n");
    exit(1);
  }

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

  return 0;
}

