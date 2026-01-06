#include <pthread.h>
#include <errno.h>
#include "common.h"

_Thread_local int l = 3;

int64_t N = 500000;

void *thread_fn(void* arg) {
  int k = 0;
  for (volatile int i = 0; i < N; i++) {
    if ((i % (N/10)) == 0) {
      printf("------- Hello Thread | Self: %d, K: %d\n", (int)pthread_self(), k++);
    }
  }
  char* path = (char*) arg;
  if (!access(path, F_OK)) {
    printf("OK access rights -- Errno: %s\n", strerror(errno));
  } else {
    printf("No access rights -- Errno: %s\n", strerror(errno));
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  char path1[100], path2[100];
  if (argc == 3) {
    strcpy(path1, argv[1]);
    strcpy(path2, argv[2]);
  } else {
    printf("Error in argc(must be 3)\n");
  }
  pthread_t tid;
  if (pthread_create(&tid, NULL, thread_fn, path2)) {
    printf("Error\n");
  }
  int k = 0;
  for (volatile int i = 0; i < N*2; i++) {
    if ((i % (N/10)) == 0) {
      printf("------ Hello Parent | Self: %d, K: %d\n", (int)pthread_self(), k++);
    }
  }
  if (!access(path1, F_OK)) {
    printf("OK access rights -- Errno: %s\n", strerror(errno));
  } else {
    printf("No access rights -- Errno: %s\n", strerror(errno));
  }
  return 0;
}
