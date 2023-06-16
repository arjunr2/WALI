#include "common.h"
#include <fcntl.h>

int main() {
  int fd = open("random_file", O_RDWR);
  int flags = fcntl(fd, F_GETFL, 0);
  int ret = fcntl(fd, F_SETFL, flags);
  return 0;
}
