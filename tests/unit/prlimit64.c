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
__attribute__((__import_module__("wali"), __import_name__("SYS_prlimit64")))
long __imported_wali_prlimit64(int pid, int resource, const struct rlimit *new_limit, struct rlimit *old_limit);

int wali_prlimit64(int pid, int resource, const struct rlimit *new_limit, struct rlimit *old_limit) { 
    return (int)__imported_wali_prlimit64(pid, resource, new_limit, old_limit); 
}
#else
#include <sys/syscall.h>
int wali_prlimit64(int pid, int resource, const struct rlimit *new_limit, struct rlimit *old_limit) { 
    return syscall(SYS_prlimit64, pid, resource, new_limit, old_limit); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Test prlimit64 (get)
    struct rlimit old;
    if (wali_prlimit64(0, RLIMIT_NOFILE, NULL, &old) != 0) return -1;
    
    if (old.rlim_cur == 0) return -1; // Sanity check, usually it's non-zero
 
    return 0;
}
