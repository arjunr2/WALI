#include "common.h"

int main() {
  struct utimbuf times = {
    .actime = 4325432,
    .modtime = 2435
  };
  if (utime("compile-wali.sh", &times)) {
    print("ERROR\n");
    exit(1);
  }
  return 0;
}
