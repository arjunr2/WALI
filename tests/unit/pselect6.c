// CMD: args="basic"

#include "wali_start.c"
#include <sys/select.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_pselect6")))
long __imported_wali_pselect6(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, void *sigmask);
__attribute__((__import_module__("wali"), __import_name__("SYS_pipe")))
long __imported_wali_pipe(int *pipefd);
__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_pselect6(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, void *sigmask) { 
    return (int)__imported_wali_pselect6(nfds, readfds, writefds, exceptfds, timeout, sigmask); 
}
int wali_pipe(int *pipefd) { return (int)__imported_wali_pipe(pipefd); }
ssize_t wali_write(int fd, const void *buf, size_t count) { return (ssize_t)__imported_wali_write(fd, buf, count); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_pselect6(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, void *sigmask) { 
    return syscall(SYS_pselect6, nfds, readfds, writefds, exceptfds, timeout, sigmask); 
}
int wali_pipe(int *pipefd) { return syscall(SYS_pipe, pipefd); }
ssize_t wali_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int pfd[2];
    if (wali_pipe(pfd) != 0) return -1;
    
    fd_set rfds;
    
    // Timeout immediately
    FD_ZERO(&rfds);
    FD_SET(pfd[0], &rfds);
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000 };
    
    int ret = wali_pselect6(pfd[0] + 1, &rfds, 0, 0, &ts, 0);
    if (ret != 0) {
        wali_close(pfd[0]);
        wali_close(pfd[1]);
        return -1;
    }
    
    // Write data
    if (wali_write(pfd[1], "Y", 1) != 1) {
        wali_close(pfd[0]);
        wali_close(pfd[1]);
        return -1;
    }
    
    // Check data available
    FD_ZERO(&rfds);
    FD_SET(pfd[0], &rfds);
    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    
    ret = wali_pselect6(pfd[0] + 1, &rfds, 0, 0, &ts, 0);
    if (ret <= 0) {
        wali_close(pfd[0]);
        wali_close(pfd[1]);
        return -1;
    }
    
    if (!FD_ISSET(pfd[0], &rfds)) {
        wali_close(pfd[0]);
        wali_close(pfd[1]);
        return -1;
    }
    
    wali_close(pfd[0]);
    wali_close(pfd[1]);
    return 0;
}
