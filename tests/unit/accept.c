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
__attribute__((__import_module__("wali"), __import_name__("SYS_listen")))
long __imported_wali_listen(int sockfd, int backlog);
__attribute__((__import_module__("wali"), __import_name__("SYS_accept")))
long __imported_wali_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
__attribute__((__import_module__("wali"), __import_name__("SYS_connect")))
long __imported_wali_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);
__attribute__((__import_module__("wali"), __import_name__("SYS_fork")))
long __imported_wali_fork(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_exit")))
long __imported_wali_exit(int status);
__attribute__((__import_module__("wali"), __import_name__("SYS_wait4")))
long __imported_wali_wait4(int pid, int *status, int options, void *rusage);

int wali_socket(int domain, int type, int protocol) { return (int)__imported_wali_socket(domain, type, protocol); }
int wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return (int)__imported_wali_bind(sockfd, addr, addrlen); }
int wali_listen(int sockfd, int backlog) { return (int)__imported_wali_listen(sockfd, backlog); }
int wali_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { return (int)__imported_wali_accept(sockfd, addr, addrlen); }
int wali_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return (int)__imported_wali_connect(sockfd, addr, addrlen); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
int wali_fork(void) { return (int)__imported_wali_fork(); }
void wali_exit(int status) { __imported_wali_exit(status); }
int wali_wait4(int pid, int *status, int options, void *rusage) { return (int)__imported_wali_wait4(pid, status, options, rusage); }

#else
#include <sys/syscall.h>
#include <sys/wait.h>
int wali_socket(int domain, int type, int protocol) { return syscall(SYS_socket, domain, type, protocol); }
int wali_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return syscall(SYS_bind, sockfd, addr, addrlen); }
int wali_listen(int sockfd, int backlog) { return syscall(SYS_listen, sockfd, backlog); }
int wali_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { return syscall(SYS_accept, sockfd, addr, addrlen); }
int wali_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return syscall(SYS_connect, sockfd, addr, addrlen); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
int wali_fork(void) { return syscall(SYS_fork); }
void wali_exit(int status) { syscall(SYS_exit, status); }
int wali_wait4(int pid, int *status, int options, void *rusage) { return syscall(SYS_wait4, pid, status, options, rusage); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int lsock = wali_socket(AF_INET, SOCK_STREAM, 0);
    if (lsock < 0) return -1;
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    
    if (wali_bind(lsock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        wali_close(lsock);
        return -1;
    }
    
    if (wali_listen(lsock, 1) != 0) {
        wali_close(lsock);
        return -1;
    }
    
    // Get assigned port
    socklen_t len = sizeof(addr);
    if (getsockname(lsock, (struct sockaddr*)&addr, &len) != 0) {
        wali_close(lsock);
        return -1;
    }
    
    int pid = wali_fork();
    if (pid < 0) {
        wali_close(lsock);
        return -1;
    }
    
    if (pid == 0) {
        // Child: connect
        wali_close(lsock);
        int csock = wali_socket(AF_INET, SOCK_STREAM, 0);
        if (csock < 0) wali_exit(1);
        
        if (wali_connect(csock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
            wali_close(csock);
            wali_exit(1);
        }
        wali_close(csock);
        wali_exit(0);
    }
    
    // Parent: accept
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int asock = wali_accept(lsock, (struct sockaddr*)&client_addr, &client_len);
    
    int status;
    wali_wait4(pid, &status, 0, 0);
    
    wali_close(lsock);
    if (asock >= 0) wali_close(asock);
    
    if (asock < 0) return -1;
    
    return 0;
}
