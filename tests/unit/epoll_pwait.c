// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/epoll.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_epoll_create1")))
long __imported_wali_epoll_create1(int flags);

__attribute__((__import_module__("wali"), __import_name__("SYS_epoll_ctl")))
long __imported_wali_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

__attribute__((__import_module__("wali"), __import_name__("SYS_epoll_pwait")))
long __imported_wali_epoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const void *sigmask, int sigsetsize);

int wali_epoll_create1(int flags) { return (int)__imported_wali_epoll_create1(flags); }
int wali_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) { return (int)__imported_wali_epoll_ctl(epfd, op, fd, event); }
int wali_epoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout) { 
    return (int)__imported_wali_epoll_pwait(epfd, events, maxevents, timeout, NULL, 8); 
}
#else
#include <sys/syscall.h>
int wali_epoll_create1(int flags) { return syscall(SYS_epoll_create1, flags); }
int wali_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) { return syscall(SYS_epoll_ctl, epfd, op, fd, event); }
int wali_epoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout) { 
    return syscall(SYS_epoll_pwait, epfd, events, maxevents, timeout, NULL, 8); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int epfd = wali_epoll_create1(0);
    if (epfd < 0) return -1;
    
    int pfd[2];
    if (pipe(pfd) != 0) return -1;
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = pfd[0];
    
    if (wali_epoll_ctl(epfd, EPOLL_CTL_ADD, pfd[0], &ev) != 0) return -1;
    
    struct epoll_event events[2];
    
    // Wait for event (should timeout 0ms)
    int n = wali_epoll_pwait(epfd, events, 2, 0);
    if (n != 0) return -1;
    
    // Write data
    if (write(pfd[1], "X", 1) != 1) return -1;
    
    // Wait for event (should trigger)
    n = wali_epoll_pwait(epfd, events, 2, 1000);
    if (n != 1) return -1;
    if (events[0].data.fd != pfd[0]) return -1;
    
    close(pfd[0]);
    close(pfd[1]);
    close(epfd);
    
    return 0;
}
