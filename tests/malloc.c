#include "common.h"
#define SIZE 1523

int main() {
  int *f = (int*) malloc(SIZE*sizeof(int));
  int *g = (int*) malloc(SIZE*sizeof(int));
  for (int i = 0; i < SIZE; i++) {
    printf("Setting elem %i\n", i);
    f[i] = 33 * i;
    g[i] = 31 * i;
  }
  int64_t res = 0;
  for (int i = 0; i < SIZE; i++) {
    res += (f[i] * g[i]);
  }
  printf("Dot product: %lld\n", res);
  free(g);
  free(f);
  return 0;
}
