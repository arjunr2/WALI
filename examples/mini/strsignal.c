#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv) {
  printf("Signal 39: %s\n", strsignal(39));
  kill(getpid(), 39);
  return 0;
}
