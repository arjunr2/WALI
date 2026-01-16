// CMD: args="basic"

#include "wali_start.c"
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_sigaltstack")))
long __imported_wali_sigaltstack(const stack_t *ss, stack_t *old_ss);

int wali_sigaltstack(const stack_t *ss, stack_t *old_ss) { 
    return (int)__imported_wali_sigaltstack(ss, old_ss); 
}

#else
#include <sys/syscall.h>
int wali_sigaltstack(const stack_t *ss, stack_t *old_ss) { 
    return syscall(SYS_sigaltstack, ss, old_ss); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // Allocate new stack
    char *stack_mem = (char*)malloc(SIGSTKSZ);
    if (!stack_mem) return -1;
    
    stack_t ss;
    ss.ss_sp = stack_mem;
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    
    if (wali_sigaltstack(&ss, 0) != 0) {
        free(stack_mem);
        return -1;
    }
    
    // Verify it was set
    stack_t current;
    if (wali_sigaltstack(0, &current) != 0) {
        free(stack_mem);
        return -1;
    }
    
    if (current.ss_sp != stack_mem) {
        free(stack_mem);
        return -1;
    }
    
    // Disable alt stack
    ss.ss_flags = SS_DISABLE;
    wali_sigaltstack(&ss, 0);
    
    free(stack_mem);
    return 0;
}
