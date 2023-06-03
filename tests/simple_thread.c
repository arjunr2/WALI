#include <pthread.h>
#include <stdio.h>

_Thread_local int v = 2;

void *thread_routine(void *arg) {
  printf("Val is %d -- %d\n", *((int*)arg), v);
  return NULL;
}

int main() {
 pthread_t tid[10];
 int val = 42;
 int stat;
 for (int i = 0; i < 10; i++) {
   if (stat = pthread_create(tid + i, NULL, thread_routine, &val)) {
     printf("Pthread create error: %d\n", stat);
   }
 }
 for (int i = 0; i < 10; i++) {
  pthread_join(tid[i], NULL);
 }
 return 0;
}
