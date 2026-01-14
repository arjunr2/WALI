// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/select.h>
#include <string.h>
#include <sys/time.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_select")))
long __imported_wali_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
__attribute__((__import_module__("wali"), __import_name__("SYS_pipe")))
long __imported_wali_pipe(int *pipefd);
__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) { 
    return (int)__imported_wali_select(nfds, readfds, writefds, exceptfds, timeout); 
}
int wali_pipe(int *pipefd) { return (int)__imported_wali_pipe(pipefd); }
int wali_write(int fd, const void *buf, size_t count) { return (int)__imported_wali_write(fd, buf, count); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) { 
    return syscall(SYS_select, nfds, readfds, writefds, exceptfds, timeout); 
}
int wali_pipe(int *pipefd) { return syscall(SYS_pipe, pipefd); }
int wali_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int pfd[2];
    if (wali_pipe(pfd) != 0) return -1;
    
    fd_set rfds;
    struct timeval tv;
    
    // 1. Check no data
    FD_ZERO(&rfds);
    FD_SET(pfd[0], &rfds);
    
    tv.tv_sec = 0;
    tv.tv_usec = 1000;
    
    int retval = wali_select(pfd[0] + 1, &rfds, NULL, NULL, &tv);
    if (retval != 0) return -1; // Should timeout
    
    // 2. Write data
    if (wali_write(pfd[1], "A", 1) != 1) return -1;
    
    // 3. Check data available
    FD_ZERO(&rfds);
    FD_SET(pfd[0], &rfds);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    
    retval = wali_select(pfd[0] + 1, &rfds, NULL, NULL, &tv);
    if (retval <= 0) return -1;
    
    if (!FD_ISSET(pfd[0], &rfds)) return -1;
    
    wali_close(pfd[0]);
    wali_close(pfd[1]);
    return 0;
}
