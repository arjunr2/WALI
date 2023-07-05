#include "common.h"
#include <pthread.h>

void sigint_handler(int signo) {
  printf("interrupt\n");
}

void *thread_fn(void *arg) {
  raise(SIGINT);
}

int main() {
  struct sigaction act = {0};
  act.sa_handler = sigint_handler;

  act.sa_flags = SA_RESTART;
  sigemptyset (&act.sa_mask);
  if (sigaction(SIGINT, &act, NULL) == -1) {
    printf("Could not install signal\n");
    exit(1);
  }
  pthread_t tid;
  if (pthread_create(&tid, NULL, thread_fn, NULL)) {
    printf("Error thread\n");
  }
  pthread_join(tid, NULL);
  printf("Done\n");
}
