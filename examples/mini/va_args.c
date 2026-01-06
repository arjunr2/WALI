#include <stdarg.h>
#include <stdio.h>

typedef long ty;

int test_args(int n, ...) {
  va_list ap;
  ty a, b, c, d, e, f, g;
  va_start(ap, n);
  a=va_arg(ap, ty);
  b=va_arg(ap, ty);
  c=va_arg(ap, ty);
  d=va_arg(ap, ty);
  e=va_arg(ap, ty);
  f=va_arg(ap, ty);
  g=va_arg(ap, ty);
  va_end(ap);
  printf("Args -- n: %d, a:%0lx, b:%0lx, c:%0lx, d:%0lx, e:%0lx, f:%0lx, g:%0lx\n", n, a, b, c, d, e, f, g);
  printf("Args -- n: %d, a:%ld, b:%ld, c:%ld, d:%ld, e:%ld, f:%ld, g:%ld\n", n, a, b, c, d, e, f, g);
  return 0; 
}

int main() {
  ty a = 33, b = 34, c = 35, d = 36, e = 37, f = 38, g = (39ULL<<32) + 40;
  int n = 7;
  volatile int l = 0x55;
  return test_args(n, a, b, c, d, e, f, g);
}
