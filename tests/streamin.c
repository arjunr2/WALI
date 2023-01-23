#include "common.h"
#include <fcntl.h>
#define SZ 100

int main() {
  char buf[SZ] = {0};
  ssize_t read_ct = read(0, buf, SZ-1);
  PRINT_INT("Read ct", read_ct);
  for (int i = 0; i < SZ; i++) {
    print_char(buf[i]);
  }
  print("\n");
  return 0;
}
