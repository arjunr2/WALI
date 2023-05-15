#include "common.h"
#include <math.h>

int main() {
  double d = 3.78;
  double d3 = d * 3;
  double f = floor(d3);
  printf("Orig: %lf, Floor: %lf\n", d3, f);
  return 0;
}
