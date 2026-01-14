// CMD: args="check_getsockname"

#include "wali_start.c"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_socket")))
long __imported_wali_socket(int domain, int type, int protocol);
__attribute__((__import_module__("wali"), __import_name__("SYS_bind")))
long __imported_wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
__attribute__((__import_module__("wali"), __import_name__("SYS_getsockname")))
long __imported_wali_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_socket(int domain, int type, int protocol) { return (int)__imported_wali_socket(domain, type, protocol); }
int wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return (int)__imported_wali_bind(sockfd, addr, addrlen); }
int wali_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { return (int)__imported_wali_getsockname(sockfd, addr, addrlen); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_socket(int domain, int type, int protocol) { return syscall(SYS_socket, domain, type, protocol); }
int wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return syscall(SYS_bind, sockfd, addr, addrlen); }
int wali_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { return syscall(SYS_getsockname, sockfd, addr, addrlen); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // We need to bind to specific port/addr to check getsockname reliably? 
    // Or just bind to 0 and check logic.
    
    int fd = wali_socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return -1;
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    
    if (wali_bind(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        wali_close(fd); return -1;
    }
    
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    memset(&name, 0, sizeof(name));
    
    if (wali_getsockname(fd, (struct sockaddr*)&name, &namelen) != 0) {
        wali_close(fd); return -1;
    }
    
    if (name.sin_family != AF_INET) { wali_close(fd); return -1; }
    // namelen should be sizeof(sockaddr_in) usually, but might vary slightly by platform if not strict. 
    // Usually it updates value.
    if (namelen != sizeof(name)) { 
        // Strict check.
        wali_close(fd); return -1; 
    }
    
    wali_close(fd);
    return 0;
}
