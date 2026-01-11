// CMD: setup="create /tmp/pipe_fifo" args="fifo /tmp/pipe_fifo"

#include "wali_start.c"
#include <unistd.h>
#include <string.h>

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
__attribute__((__import_module__("wali"), __import_name__("SYS_read")))
long __imported_wali_read(int fd, void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_pipe(int *pipefd) { return (int)__imported_wali_pipe(pipefd); }
int wali_read(int fd, void *buf, size_t count) { return (int)__imported_wali_read(fd, buf, count); }
int wali_write(int fd, const void *buf, size_t count) { return (int)__imported_wali_write(fd, buf, count); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_pipe(int *pipefd) { return syscall(SYS_pipe, pipefd); }
int wali_read(int fd, void *buf, size_t count) { return syscall(SYS_read, fd, buf, count); }
int wali_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  
  int pipefd[2];
  if (wali_pipe(pipefd) != 0) return -1;
  
  const char *msg = "PIPE_TEST";
  if (wali_write(pipefd[1], msg, 9) != 9) {
      wali_close(pipefd[0]); wali_close(pipefd[1]);
      return -1;
  }
  
  char buf[32];
  if (wali_read(pipefd[0], buf, 32) != 9) {
      wali_close(pipefd[0]); wali_close(pipefd[1]);
      return -1;
  }
  
  if (strcmp(buf, msg) != 0) {
      wali_close(pipefd[0]); wali_close(pipefd[1]);
      return -1;
  }
  
  wali_close(pipefd[0]);
  wali_close(pipefd[1]);
  
  return 0;
}
