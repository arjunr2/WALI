// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_setrlimit")))
long __imported_wali_setrlimit(int resource, const struct rlimit *rlim);
__attribute__((__import_module__("wali"), __import_name__("SYS_getrlimit")))
long __imported_wali_getrlimit(int resource, struct rlimit *rlim);

int wali_setrlimit(int resource, const struct rlimit *rlim) { return (int)__imported_wali_setrlimit(resource, rlim); }
int wali_getrlimit(int resource, struct rlimit *rlim) { return (int)__imported_wali_getrlimit(resource, rlim); }

#else
#include <sys/syscall.h>
int wali_setrlimit(int resource, const struct rlimit *rlim) { return syscall(SYS_setrlimit, resource, rlim); }
int wali_getrlimit(int resource, struct rlimit *rlim) { return syscall(SYS_getrlimit, resource, rlim); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    struct rlimit rlim;
    if (wali_getrlimit(RLIMIT_NOFILE, &rlim) != 0) return -1;
    
    // Try to lower the soft limit
    struct rlimit new_rlim = rlim;
    if (new_rlim.rlim_cur > 0) {
        new_rlim.rlim_cur -= 1;
    }
    
    if (wali_setrlimit(RLIMIT_NOFILE, &new_rlim) != 0) return -1;
    
    struct rlimit check_rlim;
    if (wali_getrlimit(RLIMIT_NOFILE, &check_rlim) != 0) return -1;
    
    if (check_rlim.rlim_cur != new_rlim.rlim_cur) return -1;
    
    return 0;
}
