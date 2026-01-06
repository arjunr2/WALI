#include "common.h"
#define PAGESIZE sysconf(_SC_PAGE_SIZE)

int main() {
  int fd = open("sample_text.txt", O_RDONLY);
  if (fd == -1) {
    perror("Failed to open \'sample_text.txt\'");
    return 1;
  }
  off_t offset = PAGESIZE;
  int length = 600900;
  printf("page size: %ld\n", PAGESIZE);
  char* addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, offset);
  if (addr == MAP_FAILED) {
    printf("Failed to mmap!\n");
    return 0;
  }
  char* addr2 = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, offset + 8 * PAGESIZE);
  if (addr2 == MAP_FAILED) {
    printf("Failed to mmap!\n");
    return 0;
  }
  int s = write(STDOUT_FILENO, addr, length);
  print("\n");
  munmap(addr2, length);
  munmap(addr, length);
  close(fd);
}
