// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_ioctl")))
long __imported_wali_ioctl(int fd, int request, void* arg);

int wali_ioctl(int fd, unsigned long request, void* arg) { 
    return (int)__imported_wali_ioctl(fd, request, arg); 
}
#else
#include <sys/syscall.h>
int wali_ioctl(int fd, unsigned long request, void* arg) { 
    return syscall(SYS_ioctl, fd, request, arg); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // We use socketpair + FIONBIO to test ioctl side effect.
    // 1. Create socketpair
    // 2. Set non-blocking on one end
    // 3. Try to read from it (should return -1 with EAGAIN immediately)
    // 4. (Control) If it was blocking, it would hang (we won't test that directly to avoid hangs, but the success of step 3 proves ioctl worked)
    
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) return -1;
    
    int on = 1;
    if (wali_ioctl(sv[0], FIONBIO, &on) != 0) {
        close(sv[0]);
        close(sv[1]);
        return -1;
    }
    
    char buf[1];
    // Should be non-blocking now
    int res = read(sv[0], buf, 1);
    
    // Expect -1 (EAGAIN)
    if (res != -1) {
        // If it read 0, that means EOF (not expected).
        // If it read 1, well, nothing was written.
        close(sv[0]);
        close(sv[1]);
        return -1;
    }
    
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
         close(sv[0]);
         close(sv[1]);
         return -1;
    }
    
    // Cleanup
    close(sv[0]);
    close(sv[1]);
    
    return 0;
}
