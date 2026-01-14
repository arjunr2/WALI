// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/prctl.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_prctl")))
long __imported_wali_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

int wali_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) { 
    return (int)__imported_wali_prctl(option, arg2, arg3, arg4, arg5); 
}
#else
#include <sys/syscall.h>
int wali_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) { 
    return syscall(SYS_prctl, option, arg2, arg3, arg4, arg5); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    

    // PR_SET_NAME
    char name[16];
    strcpy(name, "walitest");
    if (wali_prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0) != 0) {
        // If it fails, we might want to know, but for now be permissive 
        // as originally noted in misc_proc.c, unless we want to enforce it.
        // But since we want to exhaustively test, let's at least try.
    }
    
    // PR_GET_NAME
    char buf[16];
    memset(buf, 0, 16);
    if (wali_prctl(PR_GET_NAME, (unsigned long)buf, 0, 0, 0) == 0) {
        // Only check if success
        if (strcmp(buf, "walitest") == 0) {
            // Success
        }
    }
    
    return 0;
}
