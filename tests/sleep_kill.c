#include "common.h"

int main() {
  struct timespec sleep_ts = {
    .tv_sec = 60,
    .tv_nsec = 0
  };
  PRINT_INT("PID", getpid());
  struct timespec rem_ts = {0};

  nanosleep(&sleep_ts, &rem_ts);

  return 1;
}
