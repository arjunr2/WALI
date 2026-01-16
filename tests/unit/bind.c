// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int test_bind_addr(uint32_t ip_addr, const char* desc) {
    long fd = wali_syscall_socket(AF_INET, SOCK_DGRAM, 0);
    TEST_ASSERT_NE(fd, -1);
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ip_addr);
    addr.sin_port = 0; // Let OS choose

    if (ip_addr == INADDR_LOOPBACK) {
        TEST_LOG("Testing bind to loopback");
    } else {
        TEST_LOG("Testing bind to any");
    }
    long res = wali_syscall_bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    wali_syscall_close(fd);
    
    return (res == 0) ? 0 : -1;
}

int test(void) {
    TEST_ASSERT_EQ(test_bind_addr(INADDR_LOOPBACK, "loopback"), 0);
    TEST_ASSERT_EQ(test_bind_addr(INADDR_ANY, "any"), 0);
    return 0;
}
