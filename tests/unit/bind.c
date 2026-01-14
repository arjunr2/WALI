// CMD: args="bind_inet_loopback"
// CMD: args="bind_inet_any"

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
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_socket(int domain, int type, int protocol) { return (int)__imported_wali_socket(domain, type, protocol); }
int wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return (int)__imported_wali_bind(sockfd, addr, addrlen); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_socket(int domain, int type, int protocol) { return syscall(SYS_socket, domain, type, protocol); }
int wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return syscall(SYS_bind, sockfd, addr, addrlen); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *mode = argv[0];
    
    int fd = wali_socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return -1;
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    
    if (strcmp(mode, "bind_inet_loopback") == 0) {
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = 0; // Let OS choose
    } else if (strcmp(mode, "bind_inet_any") == 0) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = 0;
    } else {
        wali_close(fd);
        return -1;
    }
    
    if (wali_bind(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        wali_close(fd); return -1;
    }
    
    wali_close(fd);
    return 0;
}
