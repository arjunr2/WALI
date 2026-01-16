// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
// #include <unistd.h>
// #include <sys/epoll.h>

#include <sys/epoll.h>
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
WALI_IMPORT("SYS_epoll_create1") long wali_syscall_epoll_create1(int flags);
WALI_IMPORT("SYS_epoll_ctl") long wali_syscall_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
WALI_IMPORT("SYS_epoll_pwait") long wali_syscall_epoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const void *sigmask, int sigsetsize);

int wali_epoll_create1(int flags) { return (int)wali_syscall_epoll_create1(flags); }
int wali_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) { return (int)wali_syscall_epoll_ctl(epfd, op, fd, event); }
int wali_epoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout) { 
    return (int)wali_syscall_epoll_pwait(epfd, events, maxevents, timeout, NULL, 8); 
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
    int epfd = wali_epoll_create1(0);
    TEST_ASSERT(epfd >= 0);
    
    int pfd[2];
    TEST_ASSERT_EQ(pipe(pfd), 0);
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = pfd[0];
    
    TEST_ASSERT_EQ(wali_epoll_ctl(epfd, EPOLL_CTL_ADD, pfd[0], &ev), 0);
    
    struct epoll_event events[2];
    
    // Wait for event (should timeout 0ms)
    int n = wali_epoll_pwait(epfd, events, 2, 0);
    TEST_ASSERT_EQ(n, 0);
    
    // Write data
    TEST_ASSERT_EQ(write(pfd[1], "X", 1), 1);
    
    // Wait for event (should trigger)
    n = wali_epoll_pwait(epfd, events, 2, 1000);
    TEST_ASSERT_EQ(n, 1);
    TEST_ASSERT_EQ(events[0].data.fd, pfd[0]);
    
    wali_syscall_close(pfd[0]);
    wali_syscall_close(pfd[1]);
    wali_syscall_close(epfd);
    
    return 0;
}

