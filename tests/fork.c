#include "common.h"

int main() {
  int pid = fork();
  if (pid == 0) {
    printf("One proc\n");
  } else {
    printf("Other proc\n");
  }
  return 0;
}
