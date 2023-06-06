#include <pthread.h>
#include "common.h"

_Thread_local int l = 3;

int64_t N = 500000;

void *thread_fn(void* arg) {
  int k = 0;
  for (volatile int i = 0; i < N; i++) {
    if ((i % (N/10)) == 0) {
      printf("------- Hello Thread | Self: %d, K: %d\n", pthread_self(), k++);
      //PRINT_INT("---------- Hello Thread | Self", (long)pthread_self());
    }
  }
  return NULL;
}

int main() {
  pthread_t tid;
  if (pthread_create(&tid, NULL, thread_fn, NULL)) {
    printf("Error\n");
  }
  int k = 0;
  for (volatile int i = 0; i < N*2; i++) {
    if ((i % (N/10)) == 0) {
      printf("------ Hello Parent | Self: %d, K: %d\n", pthread_self(), k++);
      //PRINT_INT("------ Hello Parent | Self", (long)pthread_self());
    }
  }
  //sleep(6);
  //if (pthread_join(tid, NULL)) {
  //  printf("Join error\n");
  //}
  return 0;
}
