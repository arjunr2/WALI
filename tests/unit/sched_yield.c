// CMD: setup="" args=""

#include "wali_start.c"
#include <sched.h> // for sched_yield declarations in host

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_sched_yield")))
long long __imported_wali_sched_yield(void);
int wali_sched_yield(void) { return (int)__imported_wali_sched_yield(); }
#else
#include <sys/syscall.h>
int wali_sched_yield(void) { return syscall(SYS_sched_yield); }
#endif

int test(void) {
    if (wali_sched_yield() != 0) return -1;
    return 0;
}
