// CMD: setup="" args=""

#include "wali_start.c"
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_epoll_create1")))
long long __imported_wali_epoll_create1(int flags);

__attribute__((__import_module__("wali"), __import_name__("SYS_epoll_ctl")))
long long __imported_wali_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

__attribute__((__import_module__("wali"), __import_name__("SYS_epoll_pwait")))
long long __imported_wali_epoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const void *sigmask, int sigsetsize);

int wali_epoll_create1(int flags) { return (int)__imported_wali_epoll_create1(flags); }
int wali_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) { return (int)__imported_wali_epoll_ctl(epfd, op, fd, event); }
int wali_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) { 
    return (int)__imported_wali_epoll_pwait(epfd, events, maxevents, timeout, NULL, 8); 
}

#else
#include <sys/syscall.h>
// SYS_epoll_create1 is 291
// SYS_epoll_ctl is 233
// SYS_epoll_pwait is 281
int wali_epoll_create1(int flags) { return syscall(SYS_epoll_create1, flags); }
int wali_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) { return syscall(SYS_epoll_ctl, epfd, op, fd, event); }
int wali_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) { 
    return syscall(SYS_epoll_pwait, epfd, events, maxevents, timeout, NULL, 8); // size of sigset_t?
}
#endif

int test(void) {
    int pfd[2];
    if (pipe(pfd) != 0) return -1;
    
    int epfd = wali_epoll_create1(0);
    if (epfd < 0) return -1;
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = pfd[0];
    
    if (wali_epoll_ctl(epfd, EPOLL_CTL_ADD, pfd[0], &ev) != 0) return -1;
    
    // No data yet, wait with 0 timeout
    struct epoll_event events[2];
    int n = wali_epoll_wait(epfd, events, 2, 0);
    if (n != 0) return -1;
    
    // Write data
    if (write(pfd[1], "X", 1) != 1) return -1;
    
    // Wait
    n = wali_epoll_wait(epfd, events, 2, 1000);
    if (n != 1) return -1;
    if (events[0].data.fd != pfd[0]) return -1;
    if (!(events[0].events & EPOLLIN)) return -1;
    
    close(epfd);
    close(pfd[0]);
    close(pfd[1]);
    return 0;
}
