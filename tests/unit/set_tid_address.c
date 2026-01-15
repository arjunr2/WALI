// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_set_tid_address")))
long __imported_wali_set_tid_address(int *tidptr);

long wali_set_tid_address(int *tidptr) { return __imported_wali_set_tid_address(tidptr); }

#else
#include <sys/syscall.h>
long wali_set_tid_address(int *tidptr) { return syscall(SYS_set_tid_address, tidptr); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int tid_storage = 0;
    long tid = wali_set_tid_address(&tid_storage);
    
    // Should return current thread id
    if (tid <= 0) return -1;
    
    return 0;
}
