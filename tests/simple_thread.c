#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

_Thread_local int v = 2;

static int global_val = 35;

void *thread_routine(void *arg) {
  printf("Val is %d -- %d\n", *((int*)arg), v);
  return &global_val;
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
  int *res;
  pthread_join(tid[i], &res);
  printf("res join: %d\n", *res);
 }
 return 0;
}
