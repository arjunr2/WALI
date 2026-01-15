// CMD: args="basic"

#include "wali_start.c"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_socket")))
long __imported_wali_socket(int domain, int type, int protocol);
__attribute__((__import_module__("wali"), __import_name__("SYS_bind")))
long __imported_wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
__attribute__((__import_module__("wali"), __import_name__("SYS_sendto")))
long __imported_wali_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
__attribute__((__import_module__("wali"), __import_name__("SYS_recvfrom")))
long __imported_wali_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_socket(int domain, int type, int protocol) { return (int)__imported_wali_socket(domain, type, protocol); }
int wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return (int)__imported_wali_bind(sockfd, addr, addrlen); }
ssize_t wali_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) { 
    return (ssize_t)__imported_wali_sendto(sockfd, buf, len, flags, dest_addr, addrlen); 
}
ssize_t wali_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) { 
    return (ssize_t)__imported_wali_recvfrom(sockfd, buf, len, flags, src_addr, addrlen); 
}
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_socket(int domain, int type, int protocol) { return syscall(SYS_socket, domain, type, protocol); }
int wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return syscall(SYS_bind, sockfd, addr, addrlen); }
ssize_t wali_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) { 
    return syscall(SYS_sendto, sockfd, buf, len, flags, dest_addr, addrlen); 
}
ssize_t wali_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) { 
    return syscall(SYS_recvfrom, sockfd, buf, len, flags, src_addr, addrlen); 
}
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Create two UDP sockets
    int sock1 = wali_socket(AF_INET, SOCK_DGRAM, 0);
    int sock2 = wali_socket(AF_INET, SOCK_DGRAM, 0);
    if (sock1 < 0 || sock2 < 0) return -1;
    
    struct sockaddr_in addr1, addr2;
    memset(&addr1, 0, sizeof(addr1));
    memset(&addr2, 0, sizeof(addr2));
    addr1.sin_family = AF_INET;
    addr1.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr1.sin_port = 0;
    addr2 = addr1;
    
    if (wali_bind(sock1, (struct sockaddr*)&addr1, sizeof(addr1)) != 0) goto fail;
    if (wali_bind(sock2, (struct sockaddr*)&addr2, sizeof(addr2)) != 0) goto fail;
    
    // Get port of sock2
    socklen_t len = sizeof(addr2);
    if (getsockname(sock2, (struct sockaddr*)&addr2, &len) != 0) goto fail;
    
    // Send from sock1 to sock2
    const char *msg = "HELLO";
    if (wali_sendto(sock1, msg, 5, 0, (struct sockaddr*)&addr2, sizeof(addr2)) != 5) goto fail;
    
    // Receive on sock2
    char buf[16];
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    ssize_t n = wali_recvfrom(sock2, buf, sizeof(buf), 0, (struct sockaddr*)&from, &fromlen);
    if (n != 5) goto fail;
    if (memcmp(buf, "HELLO", 5) != 0) goto fail;
    
    wali_close(sock1);
    wali_close(sock2);
    return 0;
    
fail:
    wali_close(sock1);
    wali_close(sock2);
    return -1;
}
