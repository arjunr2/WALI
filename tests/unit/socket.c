// CMD: args="udp"

#include "wali_start.c"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
__attribute__((__import_module__("wali"), __import_name__("SYS_socket")))
long long __imported_wali_socket(int domain, int type, int protocol);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long long __imported_wali_close(int fd);

int wali_socket(int domain, int type, int protocol) { return (int)__imported_wali_socket(domain, type, protocol); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_socket(int domain, int type, int protocol) { return syscall(SYS_socket, domain, type, protocol); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  
  // Basic socket creation
  int fd = wali_socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) return -1;
  
  wali_close(fd);
  
  int fd2 = wali_socket(AF_INET, SOCK_STREAM, 0);
  if (fd2 < 0) return -1;
  wali_close(fd2);
  
  return 0;
}
