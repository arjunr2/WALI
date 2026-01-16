// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <string.h>
// #include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

int test(void) {
    // Create listening socket
    int lsock = wali_syscall_socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT(lsock >= 0);
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    
    TEST_ASSERT_EQ(wali_syscall_bind(lsock, &addr, sizeof(addr)), 0);
    TEST_ASSERT_EQ(wali_syscall_listen(lsock, 5), 0);
    
    // Get assigned port
    struct sockaddr_in bound_addr;
    socklen_t len = sizeof(bound_addr);
    TEST_ASSERT_EQ(wali_syscall_getsockname(lsock, &bound_addr, &len), 0);
    
    // Create client socket and connect
    int csock = wali_syscall_socket(AF_INET, SOCK_STREAM, 0);
    if (csock < 0) {
        wali_syscall_close(lsock);
        TEST_FAIL("Failed to create client socket");
    }
    
    int res = wali_syscall_connect(csock, &bound_addr, sizeof(bound_addr));
    if (res != 0) {
        wali_syscall_close(csock);
        wali_syscall_close(lsock);
        TEST_FAIL("Connect failed");
    }
    
    wali_syscall_close(csock);
    wali_syscall_close(lsock);
    return 0;
}

