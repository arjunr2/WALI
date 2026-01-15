// CMD: args="basic"

#include "wali_start.c"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_socketpair")))
long __imported_wali_socketpair(int domain, int type, int protocol, int sv[2]);
__attribute__((__import_module__("wali"), __import_name__("SYS_sendmsg")))
long __imported_wali_sendmsg(int sockfd, const struct msghdr *msg, int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_recvmsg")))
long __imported_wali_recvmsg(int sockfd, struct msghdr *msg, int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_socketpair(int domain, int type, int protocol, int sv[2]) { return (int)__imported_wali_socketpair(domain, type, protocol, sv); }
ssize_t wali_sendmsg(int sockfd, const struct msghdr *msg, int flags) { return (ssize_t)__imported_wali_sendmsg(sockfd, msg, flags); }
ssize_t wali_recvmsg(int sockfd, struct msghdr *msg, int flags) { return (ssize_t)__imported_wali_recvmsg(sockfd, msg, flags); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_socketpair(int domain, int type, int protocol, int sv[2]) { return syscall(SYS_socketpair, domain, type, protocol, sv); }
ssize_t wali_sendmsg(int sockfd, const struct msghdr *msg, int flags) { return syscall(SYS_sendmsg, sockfd, msg, flags); }
ssize_t wali_recvmsg(int sockfd, struct msghdr *msg, int flags) { return syscall(SYS_recvmsg, sockfd, msg, flags); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int sv[2];
    if (wali_socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) return -1;
    
    // Prepare message
    char data[] = "TEST123";
    struct iovec iov;
    iov.iov_base = data;
    iov.iov_len = 7;
    
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
    ssize_t sent = wali_sendmsg(sv[0], &msg, 0);
    if (sent != 7) goto fail;
    
    wali_close(sv[0]);
    wali_close(sv[1]);
    return 0;
    
fail:
    wali_close(sv[0]);
    wali_close(sv[1]);
    return -1;
}
