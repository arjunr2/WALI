#include "common.h"

int main(int argc, char **argv) {
  volatile int x = 0;
  x++;
  char* c = getenv("TERM");
  char* d = getenv("HOME");
  printf("TERM = %s\n", c);
  printf("HOME = %s\n", d);
  return 0;
}

int unused_function() {
  int unused = 0;
  return unused;
}
