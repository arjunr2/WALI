#include "common.h"

int main() {
  struct timespec req_ts = {
    .tv_sec = 3,
    .tv_nsec = 4543
  };
  struct timespec rem_ts;
  if (clock_nanosleep(CLOCK_MONOTONIC, 0, &req_ts, &rem_ts)) {
    print("ERROR\n");
    exit(1);
  }
  return 0;
}
