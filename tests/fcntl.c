#include "common.h"
#include <fcntl.h>

int main() {
  int fd = open("random_file", O_RDWR);
  int flags = fcntl(fd, F_GETFL, 0);
  int ret = fcntl(fd, F_SETFL, flags);
  printf("Struct flock size: %d\n", sizeof(struct flock));
  return 0;
}
