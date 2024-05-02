#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#define N 100

int main() {
  char buf[N];
  printf("Enter a sentence (less than %d words)\n", N-1);
  int num_bytes = read(0, buf, N-1);
  buf[num_bytes] = 0;
  buf[0] = toupper(buf[0]);
  printf("Replaced buf: %s\n", buf);
  return 0;
}
