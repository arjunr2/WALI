// CMD: setup="" args=""

#include "wali_start.c"
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_socketpair")))
long long __imported_wali_socketpair(int domain, int type, int protocol, int *sv);
int wali_socketpair(int domain, int type, int protocol, int *sv) { return (int)__imported_wali_socketpair(domain, type, protocol, sv); }
#else
#include <sys/syscall.h>
int wali_socketpair(int domain, int type, int protocol, int *sv) { return syscall(SYS_socketpair, domain, type, protocol, sv); }
#endif

int test(void) {
    int sv[2];
    // AF_UNIX = 1 on Linux
    if (wali_socketpair(1, SOCK_STREAM, 0, sv) != 0) return -1;
    
    if (write(sv[0], "PING", 4) != 4) return -1;
    
    char buf[5];
    memset(buf, 0, 5);
    if (read(sv[1], buf, 4) != 4) return -1;
    
    if (strcmp(buf, "PING") != 0) return -1;
    
    close(sv[0]);
    close(sv[1]);
    return 0;
}
