#include "common.h"

int main() {
  write(1, "Q", 1);
  int *f = (int*) malloc(15*sizeof(int));
  printf("Malloc addr: %lld\n", f);
  for (int i = 0; i < 15; i++) {
    printf("Accessing elem %i\n", i);
    f[i] = 64 * i;
  }
  PRINT_INT("Elem 4", f[4]);
  return 0;
}
