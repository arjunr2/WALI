#include "common.h"

int main() {
  wprintf(L"Hello world: %d\n", 42);
  char *s = malloc(200);
  strcpy(s, "Mycustomstring");
  wprintf(L"Hello: %s\n", s);
  return 0;
}
