#include <stdio.h>
#include <pthread.h>

void *thread_fn(void* arg) {
  printf("Hello thread\n");
  return NULL;
}

int main() {
  pthread_t tid;
  if (pthread_create(&tid, NULL, thread_fn, NULL)) {
    printf("Error\n");
  }
  if (pthread_join(tid, NULL)) {
    printf("Join error\n");
  }
  return 0;
}
