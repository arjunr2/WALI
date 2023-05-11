#include "common.h"
#include <sys/file.h>

int main() {
  FILE* fp = fopen("sample_fileno.txt", "w");
  if (!fp) {
    printf("Failed to open file\n");
    exit(1);
  }
  int result = flock(fileno(fp), LOCK_EX);
  srand(time(NULL));
  int r = rand();
  for (int i = 0; i < 5; i++) {
    fprintf(fp, "Hello from File %d\n", r);
    sleep(1);
  }
  flock(fileno(fp), LOCK_UN);
  fclose(fp);

  return 0;
}
