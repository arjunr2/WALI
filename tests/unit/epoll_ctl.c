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

int wali_epoll_create1(int flags) { return (int)wali_syscall_epoll_create1(flags); }
int wali_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) { return (int)wali_syscall_epoll_ctl(epfd, op, fd, event); }
#else
#include <sys/syscall.h>
int wali_epoll_create1(int flags) { return syscall(SYS_epoll_create1, flags); }
int wali_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) { return syscall(SYS_epoll_ctl, epfd, op, fd, event); }
#endif

int test(void) {
    int epfd = wali_epoll_create1(0);
    TEST_ASSERT(epfd >= 0);
    
    int pfd[2];
    TEST_ASSERT_EQ(pipe(pfd), 0);
    
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = pfd[0];
    
    // Add event
    TEST_ASSERT_EQ(wali_epoll_ctl(epfd, EPOLL_CTL_ADD, pfd[0], &ev), 0);
    
    // Delete event
    TEST_ASSERT_EQ(wali_epoll_ctl(epfd, EPOLL_CTL_DEL, pfd[0], &ev), 0);
    
    wali_syscall_close(pfd[0]);
    wali_syscall_close(pfd[1]);
    wali_syscall_close(epfd);
    
    return 0;
}

