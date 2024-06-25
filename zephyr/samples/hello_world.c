#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define N 10

int main() {
  char* cp = (char*) malloc(10);
  int *s = (int*) malloc(N*sizeof(int));
  for (int i = 0; i < N; i++) {
    s[i] = 63*i;
  }
  int test_num = 35;
  strcpy(cp, "Mystring");  //+ __stdout.flags;
  printf("Hello World\n");
  printf("String: %s | Testnum: %d\n", cp, test_num);
  for (int i = 0; i < N; i++) {
    printf("Num[%d]: %d\n", i, s[i]);
  }
  return 0;
}
