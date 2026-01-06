#include "common.h"
#include <signal.h>

#define SEC 2

void custom_signal_handler(int signo) {
  printf("Custom Signal Handler: %s\n", strsignal(signo));
}

int main() {
  printf("SIG_DFL: %p\n", SIG_DFL);
  signal (SIGSEGV, SIG_DFL);
  printf("Custom handler: %p\n", custom_signal_handler);
  signal (SIGFPE, custom_signal_handler);

  printf("PID: %d\n", getpid());

  int i = 0;
  while (i++ < 100000000) {
    print("Wait 2 sec\n");
    sleep(2);
  }
  print("Unreachable\n");

  return 0;
}
