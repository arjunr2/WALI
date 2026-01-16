// CMD: setup="" args="" cleanup=""

#include "wali_start.c"

#ifdef __wasm__
WALI_IMPORT("SYS_brk") long wali_syscall_brk(void *addr);
#else
#include <sys/syscall.h>
long wali_syscall_brk(void *addr) { return syscall(SYS_brk, addr); }
#endif

int test(void) {
    // Check current break
    TEST_LOG("Testing brk(0)");
    long curr_brk = wali_syscall_brk(0);
    
    // brk(0) returns current break address, or -1 on failure (though unlikely for brk(0))
    // We treat it as a long address.
    TEST_ASSERT_NE(curr_brk, -1);

    // Attempt to grow heap is generally tricky to test portably without allocator interference,
    // but just verifying the syscall link is the main goal here.
    
    return 0;
}
