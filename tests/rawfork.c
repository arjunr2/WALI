#include "common.h"

int main() {
  int pid;
  for (int i = 0; i < 10000; i++) {
    pid = syscall(SYS_fork);
    if (pid == 0) break;
  }
  if (pid) { sleep(1); }
  return 0;
}
