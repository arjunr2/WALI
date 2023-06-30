#include "common.h"

int main(int argc, char **argv) {
  volatile int d = 0;
  d++;
  char* c = getenv("TERM");
  printf("TERM = %s\n", c);
  return 0;
}

int unused_function() {
  int unused = 0;
  return unused;
}
