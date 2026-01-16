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

int wali_epoll_create1(int flags) { return (int)__imported_wali_epoll_create1(flags); }
#else
#include <sys/syscall.h>
int wali_epoll_create1(int flags) { return syscall(SYS_epoll_create1, flags); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int epfd = wali_epoll_create1(0);
    if (epfd < 0) return -1;
    
    // Validate we got a valid fd
    if (close(epfd) != 0) return -1;
    
    return 0;
}
