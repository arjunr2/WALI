// CMD: setup="" args="" cleanup=""

#include "wali_start.c"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

int test(void) {
    int lsock = wali_syscall_socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT(lsock >= 0);
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    
    TEST_ASSERT_EQ(wali_syscall_bind(lsock, &addr, sizeof(addr)), 0);
    TEST_ASSERT_EQ(wali_syscall_listen(lsock, 1), 0);
    
    socklen_t len = sizeof(addr);
    TEST_ASSERT_EQ(wali_syscall_getsockname(lsock, &addr, &len), 0);
    
    int pid = wali_syscall_fork();
    TEST_ASSERT(pid >= 0);
    
    if (pid == 0) {
        wali_syscall_close(lsock);
        int csock = wali_syscall_socket(AF_INET, SOCK_STREAM, 0);
        if (csock < 0) wali_syscall_exit(1);
        
        if (wali_syscall_connect(csock, &addr, sizeof(addr)) != 0) {
            wali_syscall_close(csock);
            wali_syscall_exit(1);
        }
        wali_syscall_close(csock);
        wali_syscall_exit(0);
    }
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int asock = wali_syscall_accept4(lsock, &client_addr, &client_len, SOCK_CLOEXEC);
    
    int status;
    wali_syscall_wait4(pid, &status, 0, 0);
    
    int flags = -1;
    if (asock >= 0) {
        flags = wali_syscall_fcntl(asock, F_GETFD, 0);
    }
    
    wali_syscall_close(lsock);
    if (asock >= 0) wali_syscall_close(asock);
    
    TEST_ASSERT(asock >= 0);
    TEST_ASSERT(flags & FD_CLOEXEC);
    
    return 0;
}
