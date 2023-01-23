#include "common.h"

int main() {
  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts)) {
    print("ERROR\n");
    exit(1);
  }
  PRINT_INT("Sec", ts.tv_sec);
  PRINT_INT("Nsec", ts.tv_nsec);
  return 0;
}
