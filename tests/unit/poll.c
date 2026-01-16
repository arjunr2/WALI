// CMD: 

#include "wali_start.c"
#include <unistd.h>
#include <string.h>
#include <poll.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    return 0;
}

int test_cleanup(int argc, char **argv) {
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_pipe")))
long __imported_wali_pipe(int *pipefd);
__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);
__attribute__((__import_module__("wali"), __import_name__("SYS_poll")))
long long __imported_wali_poll(struct pollfd *fds, nfds_t nfds, int timeout);

int wali_pipe(int *pipefd) { return (int)__imported_wali_pipe(pipefd); }
int wali_write(int fd, const void *buf, size_t count) { return (int)__imported_wali_write(fd, buf, count); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
int wali_poll(struct pollfd *fds, nfds_t nfds, int timeout) { return (int)__imported_wali_poll(fds, (int)nfds, timeout); }

#else
#include <sys/syscall.h>
int wali_pipe(int *pipefd) { return syscall(SYS_pipe, pipefd); }
int wali_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
int wali_poll(struct pollfd *fds, nfds_t nfds, int timeout) { return syscall(SYS_poll, fds, nfds, timeout); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  
  int pipefd[2];
  if (wali_pipe(pipefd) != 0) return -1;
  
  struct pollfd pfd;
  pfd.fd = pipefd[0];
  pfd.events = POLLIN;
  pfd.revents = 0;
  
  // 1. Poll empty pipe with timeout 10ms -> Should return 0 (timeout)
  int res = wali_poll(&pfd, 1, 10);
  if (res != 0) {
      wali_close(pipefd[0]); wali_close(pipefd[1]);
      return -1;
  }
  
  // 2. Write to pipe
  if (wali_write(pipefd[1], "A", 1) != 1) {
      wali_close(pipefd[0]); wali_close(pipefd[1]);
      return -1;
  }
  
  // 3. Poll again -> Should return 1 (ready)
  pfd.revents = 0;
  res = wali_poll(&pfd, 1, 100);
  if (res != 1) {
     wali_close(pipefd[0]); wali_close(pipefd[1]);
     return -1;
  }
  if ((pfd.revents & POLLIN) == 0) {
     wali_close(pipefd[0]); wali_close(pipefd[1]);
     return -1;
  }
  
  wali_close(pipefd[0]);
  wali_close(pipefd[1]);
  return 0;
}
