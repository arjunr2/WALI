#include "common.h"
#include <sys/epoll.h>

int main() {
  printf("Size of epoll event: %d\n", sizeof(struct epoll_event));
  return 0;
}
