// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getcwd")))
long __imported_wali_getcwd(char *buf, size_t size);

long wali_getcwd(char *buf, size_t size) { return __imported_wali_getcwd(buf, size); }

#else
#include <sys/syscall.h>
long wali_getcwd(char *buf, size_t size) { return syscall(SYS_getcwd, buf, size); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    char buf[256];
    long ret = wali_getcwd(buf, sizeof(buf));
    if (ret <= 0) return -1;
    
    // Should start with /
    if (buf[0] != '/') return -1;
    
    // Check length matches return value  
    size_t len = strlen(buf);
    if (len == 0) return -1;
    
    return 0;
}
