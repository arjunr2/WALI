// CMD: args="basic"

#include "wali_start.c"
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_ppoll")))
long __imported_wali_ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *tmo_p, const sigset_t *sigmask, size_t sigsetsize);
__attribute__((__import_module__("wali"), __import_name__("SYS_pipe")))
long __imported_wali_pipe(int *pipefd);
__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *tmo_p, const sigset_t *sigmask, size_t sigsetsize) { 
    return (int)__imported_wali_ppoll(fds, nfds, tmo_p, sigmask, sigsetsize); 
}
int wali_pipe(int *pipefd) { return (int)__imported_wali_pipe(pipefd); }
ssize_t wali_write(int fd, const void *buf, size_t count) { return (ssize_t)__imported_wali_write(fd, buf, count); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *tmo_p, const sigset_t *sigmask, size_t sigsetsize) { 
    return syscall(SYS_ppoll, fds, nfds, tmo_p, sigmask, sigsetsize); 
}
int wali_pipe(int *pipefd) { return syscall(SYS_pipe, pipefd); }
ssize_t wali_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int pfd[2];
    if (wali_pipe(pfd) != 0) return -1;
    
    struct pollfd fds[1];
    fds[0].fd = pfd[0];
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    
    // Timeout immediately
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000 };
    int ret = wali_ppoll(fds, 1, &ts, 0, 0);
    if (ret != 0) {
        wali_close(pfd[0]);
        wali_close(pfd[1]);
        return -1;  // Should timeout with 0
    }
    
    // Write data
    if (wali_write(pfd[1], "X", 1) != 1) {
        wali_close(pfd[0]);
        wali_close(pfd[1]);
        return -1;
    }
    
    // Now should have data
    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    ret = wali_ppoll(fds, 1, &ts, 0, 0);
    if (ret <= 0) {
        wali_close(pfd[0]);
        wali_close(pfd[1]);
        return -1;
    }
    
    if (!(fds[0].revents & POLLIN)) {
        wali_close(pfd[0]);
        wali_close(pfd[1]);
        return -1;
    }
    
    wali_close(pfd[0]);
    wali_close(pfd[1]);
    return 0;
}
