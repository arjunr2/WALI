#include <pthread.h>
#include "common.h"

int64_t N = 10000;
int64_t i = 0;

pthread_mutex_t lock;

void *thread_fn(void* arg) {
  for (volatile int j = 0; j < N; j++) {
    pthread_mutex_lock(&lock);
    i -= j;
    pthread_mutex_unlock(&lock);
  }
  printf("Child end: %d\n", i);
  return NULL;
}

int main() {
  pthread_t tid;
  
  if (pthread_create(&tid, NULL, thread_fn, NULL)) {
    printf("Create thread error\n");
  }
  if (pthread_mutex_init(&lock, NULL)) {
    printf("Create mutex error\n");
  }

  for (volatile int j = 0; j < N; j++) {
    pthread_mutex_lock(&lock);
    i += j;
    pthread_mutex_unlock(&lock);
  }
  printf("Parent end: %d\n", i);
  sleep(1);
  return 0;
}
