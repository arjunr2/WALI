// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_socket")))
long __imported_wali_socket(int domain, int type, int protocol);
__attribute__((__import_module__("wali"), __import_name__("SYS_setsockopt")))
long __imported_wali_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
__attribute__((__import_module__("wali"), __import_name__("SYS_getsockopt")))
long __imported_wali_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_socket(int domain, int type, int protocol) { return (int)__imported_wali_socket(domain, type, protocol); }
int wali_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) { 
    return (int)__imported_wali_setsockopt(sockfd, level, optname, optval, optlen); 
}
int wali_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) { 
    return (int)__imported_wali_getsockopt(sockfd, level, optname, optval, optlen); 
}
int wali_close(int fd) { return (int)__imported_wali_close(fd); }

#else
#include <sys/syscall.h>
int wali_socket(int domain, int type, int protocol) { return syscall(SYS_socket, domain, type, protocol); }
int wali_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) { 
    return syscall(SYS_setsockopt, sockfd, level, optname, optval, optlen); 
}
int wali_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) { 
    return syscall(SYS_getsockopt, sockfd, level, optname, optval, optlen); 
}
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int fd = wali_socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    
    int reuse = 1;
    if (wali_setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) {
        wali_close(fd); return -1;
    }
    
    int val = 0;
    socklen_t len = sizeof(val);
    if (wali_getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, &len) != 0) {
        wali_close(fd); return -1;
    }
    
    // On linux, SO_REUSEADDR returns the value set (boolean)
    if (val == 0) { wali_close(fd); return -1; }
    
    wali_close(fd);
    return 0;
}
