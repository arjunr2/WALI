// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_sysinfo")))
long __imported_wali_sysinfo(struct sysinfo *info);

int wali_sysinfo(struct sysinfo *info) { return (int)__imported_wali_sysinfo(info); }
#else
#include <sys/syscall.h>
int wali_sysinfo(struct sysinfo *info) { return syscall(SYS_sysinfo, info); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    struct sysinfo info;
    if (wali_sysinfo(&info) != 0) return -1;
    
    // Basic sanity checks
    if (info.mem_unit == 0) return -1;
    if (info.totalram == 0) return -1;
    // uptime should be > 0 usually
    // if (info.uptime == 0) return -1; // Technically possible if just booted
    
    return 0;
}
