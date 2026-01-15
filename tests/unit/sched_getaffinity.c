// CMD: args="basic"

#define _GNU_SOURCE
#include "wali_start.c"
#include <sched.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_sched_getaffinity")))
long __imported_wali_sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask);

int wali_sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask) { 
    return (int)__imported_wali_sched_getaffinity(pid, cpusetsize, mask); 
}

#else
#include <sys/syscall.h>
int wali_sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask) { 
    return syscall(SYS_sched_getaffinity, pid, cpusetsize, mask); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    cpu_set_t mask;
    CPU_ZERO(&mask);
    
    // Get affinity for current process
    int ret = wali_sched_getaffinity(0, sizeof(mask), &mask);
    if (ret < 0) return -1;
    
    // Should have at least one CPU
    int count = CPU_COUNT(&mask);
    if (count <= 0) return -1;
    
    return 0;
}
