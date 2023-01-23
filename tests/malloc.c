#include "common.h"

int main() {
  int *f = (int*) malloc(15*sizeof(int));
  for (int i = 0; i < 15; i++) {
    f[i] = 64 * i;
  }
  PRINT_INT("Elem 4", f[4]);
  return 0;
}
