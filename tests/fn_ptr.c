#include "common.h"

int (*p[4]) (int x, int y);
int (*vd)(int x, int y);

int sum(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int divs(int a, int b) { return a / b; }

int main() {
  vd = subtract;
  printf("VD: %p\n", vd);
  p[0] = sum;
  p[1] = subtract;
  p[2] = mul;
  p[3] = divs;
  int res = (*p[2])(3, 7);
  int res2 = (*vd)(28, 33);
  printf("Res: %d\n", res);
  printf("Res2: %d\n", res2);
  return 0;
}
