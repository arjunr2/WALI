#include "common.h"
#include <fcntl.h>
#define SZ 100

int main() {
  int fd =  open("compile-wali.sh", O_RDONLY);
  char buf[SZ] = {0};
  ssize_t read_ct = read(fd, buf, SZ-1);
  PRINT_INT("Read ct", read_ct);
  for (int i = 0; i < SZ; i++) {
    print_char(buf[i]);
  }
  print("\n");
  close(fd);
  return 0;
}
