#include "common.h"
#define PAGESIZE sysconf(_SC_PAGE_SIZE)

int main() {
  int length = 15;
  printf("page size: %ld\n", PAGESIZE);
  char* addr = mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  if (addr == MAP_FAILED) {
    printf("Failed to mmap!\n");
    return 0;
  }
  for (int i = 0; i < 15; i++) {
    printf("Accessing elem %i\n", i);
    addr[i] = 3 * i;
    printf("%d\n", addr[i]);
  }
  print("\n");
  munmap(addr, length);
}
