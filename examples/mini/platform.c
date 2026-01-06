#include <stdio.h>
#if defined(__linux__)
int main() {
  printf("Linux\n");
}
#else
int main() {
  printf("No linux\n");
}
#endif
