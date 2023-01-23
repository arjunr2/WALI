#include "common.h"

int main() {
  struct timespec sleep_ts = {
    .tv_sec = 3,
    .tv_nsec = 530093847
  };
  struct timespec rem_ts = {0};

  PRINT_INT("Timespec size", sizeof(rem_ts));
  if (nanosleep(&sleep_ts, &rem_ts) == -1) {
    print("ERROR\n");
    exit(1);
  }
  return 1;
}
