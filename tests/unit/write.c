// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_write")))
long __imported_wali_write(int fd, const void *buf, size_t count);

long wali_write(int fd, const void *buf, size_t count) { return __imported_wali_write(fd, buf, count); }
#else
#include <sys/syscall.h>
long wali_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int pfd[2];
    if (pipe(pfd) != 0) return -1;
    
    const char *msg = "WriteTest";
    long written = wali_write(pfd[1], msg, strlen(msg));
    
    if (written != (long)strlen(msg)) return -1;
    
    char buf[20];
    if (read(pfd[0], buf, strlen(msg)) != (long)strlen(msg)) return -1;
    
    if (strncmp(buf, msg, strlen(msg)) != 0) return -1;
    
    close(pfd[0]);
    close(pfd[1]);
    
    return 0;
}
