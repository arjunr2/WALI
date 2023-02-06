#include "common.h"

#define ALARM_TIME 3

int main() {
  pid_t cpid = syscall(SYS_fork);
  int ct = 0;
  /* Child */
  if (cpid == 0) {
    sigset_t block_alarm;
    sigemptyset (&block_alarm);
    sigaddset (&block_alarm, SIGALRM);

    syscall(SYS_alarm, ALARM_TIME);
    sigprocmask (SIG_BLOCK, &block_alarm, NULL);
    for (int i = 0; i < ALARM_TIME + 15; i++) {
      printf("Child ct: %d\n", ct);
      ct++;
      sleep(1);
      if (i == ALARM_TIME + 7) {
        sigprocmask (SIG_UNBLOCK, &block_alarm, NULL);
      }
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
