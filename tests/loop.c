#include "common.h"

int main(int argc, char *argv[]) {
  long long N = 0;
  if (argc > 1) {
    N = atoi(argv[1]);
  }
  volatile long long i = 0;
  while (i < N) {
    if ((i % 10000000) == 0) {
      printf("Ct 10m\n");
    }
    i++;
  }
}
