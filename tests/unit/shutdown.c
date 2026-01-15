// CMD: args="basic"

#include "wali_start.c"
#include <sys/socket.h>
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_socketpair")))
long __imported_wali_socketpair(int domain, int type, int protocol, int sv[2]);
__attribute__((__import_module__("wali"), __import_name__("SYS_shutdown")))
long __imported_wali_shutdown(int sockfd, int how);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);
__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);

int wali_socketpair(int domain, int type, int protocol, int sv[2]) { return (int)__imported_wali_socketpair(domain, type, protocol, sv); }
int wali_shutdown(int sockfd, int how) { return (int)__imported_wali_shutdown(sockfd, how); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
ssize_t wali_write(int fd, const void *buf, size_t count) { return (ssize_t)__imported_wali_write(fd, buf, count); }

#else
#include <sys/syscall.h>
int wali_socketpair(int domain, int type, int protocol, int sv[2]) { return syscall(SYS_socketpair, domain, type, protocol, sv); }
int wali_shutdown(int sockfd, int how) { return syscall(SYS_shutdown, sockfd, how); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
ssize_t wali_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int sv[2];
    if (wali_socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) return -1;
    
    // Shutdown write on sv[0]
    if (wali_shutdown(sv[0], SHUT_WR) != 0) {
        wali_close(sv[0]);
        wali_close(sv[1]);
        return -1;
    }
    
    // Write on sv[0] should fail
    ssize_t ret = wali_write(sv[0], "A", 1);
    if (ret > 0) {
        wali_close(sv[0]);
        wali_close(sv[1]);
        return -1; // Should have failed
    }
    
    wali_close(sv[0]);
    wali_close(sv[1]);
    return 0;
}
