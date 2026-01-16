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
int wali_epoll_create1(int flags) { return (int)wali_syscall_epoll_create1(flags); }
#else
#include <sys/syscall.h>
int wali_epoll_create1(int flags) { return syscall(SYS_epoll_create1, flags); }
#endif

int test(void) {
    int epfd = wali_epoll_create1(0);
    TEST_ASSERT(epfd >= 0);
    
    // Validate we got a valid fd
    TEST_ASSERT_EQ(wali_syscall_close(epfd), 0);
    
    return 0;
}

