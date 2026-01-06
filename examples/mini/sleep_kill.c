#include "common.h"

int main() {
  PRINT_INT("PID", getpid());
  sleep(60);
  return 1;
}
