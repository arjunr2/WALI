#include "common.h"

#define ALARM_TIME 3

int main() {
  pid_t cpid = fork();
  int ct = 0;
  /* Child */
  if (cpid == 0) {
    alarm(ALARM_TIME);
    while (1) {
      printf("Child ct: %d\n", ct);
      ct++;
      sleep(1);
    }
  }
  /* Parent */
  else {
    int i = 0;
    for (int i = 0; i < ALARM_TIME + 3; i++) {
      printf("Parent ct: %d\n", ct);
      ct++;
      sleep(1);
    }
  }
  return 0;
}
