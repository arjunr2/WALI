#include "common.h"
#define PAGESIZE sysconf(_SC_PAGE_SIZE)

int main() {
  int fd = open("sample_text.txt", O_RDONLY);
  off_t offset = PAGESIZE;
  int length = 60090;
  printf("page size: %ld\n", PAGESIZE);
  char* addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, offset);
  if (addr == MAP_FAILED) {
    printf("Failed to mmap!\n");
    return 0;
  }
  int s = write(STDOUT_FILENO, addr, length);
  printf("\nNum chars printed: %d\n", s);
  print("\n\n");
  /* Should only write 2 pages of data */
  mprotect(addr + 2 * PAGESIZE, 3, PROT_NONE);
  s = write(STDOUT_FILENO, addr, length);
  printf("\nNum chars printed: %d\n", s);

  munmap(addr, length);
  close(fd);
}
