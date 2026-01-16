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
__attribute__((__import_module__("wali"), __import_name__("SYS_pselect6")))
long __imported_wali_pselect6(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, const void *sigmask);

int wali_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, const void *sigmask) { 
    return (int)__imported_wali_pselect6(nfds, readfds, writefds, exceptfds, timeout, sigmask); 
}
#else
#include <sys/syscall.h>
int wali_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, const void *sigmask) { 
    return syscall(SYS_pselect6, nfds, readfds, writefds, exceptfds, timeout, sigmask); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;

    int pfd[2];
    if (pipe(pfd) != 0) return -1;

    // write data to pipe
    if (write(pfd[1], "a", 1) != 1) return -1;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(pfd[0], &rfds);

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000; // 1ms

    // Test pselect returns ready fd
    // We pass NULL for sigmask for simplicity, but use valid sets
    int retval = wali_pselect(pfd[0] + 1, &rfds, NULL, NULL, &ts, NULL);
    
    if (retval < 0) {
        close(pfd[0]);
        close(pfd[1]);
        return -1;
    }
    
    // Expect 1 ready descriptor
    if (retval != 1) {
        close(pfd[0]);
        close(pfd[1]);
        return -1;
    }

    // Expect pfd[0] to be set
    if (!FD_ISSET(pfd[0], &rfds)) {
        close(pfd[0]);
        close(pfd[1]);
        return -1;
    }
    
    close(pfd[0]);
    close(pfd[1]);
    return 0;
}
