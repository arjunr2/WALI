#include "common.h"
#include <sys/file.h>

int main() {
  FILE* fp = fopen("sample_fileno.txt", "w");
  if (!fp) {
    printf("Failed to open file\n");
    exit(1);
  }
  srand(time(NULL));
  int r = rand();
  printf("Rand no: %d\n", r);
  for (int i = 0; i < 6; i++) {
    char val[200];
    sprintf(val, "Hello from File %d\n", r);
    fwrite_unlocked(val, 1, strlen(val), fp);
    fflush_unlocked(fp);
    sleep(1);
  }
  fclose(fp);

  return 0;
}
