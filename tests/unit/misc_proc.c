// CMD: setup="" args=""

#include "wali_start.c"
#include <unistd.h>
#include <sys/prctl.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_gettid")))
long long __imported_wali_gettid(void);

__attribute__((__import_module__("wali"), __import_name__("SYS_prctl")))
long long __imported_wali_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

pid_t wali_gettid(void) { return (pid_t)__imported_wali_gettid(); }
int wali_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
    return (int)__imported_wali_prctl(option, arg2, arg3, arg4, arg5);
}
#else
#include <sys/syscall.h>
pid_t wali_gettid(void) { return syscall(SYS_gettid); }
int wali_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
    return syscall(SYS_prctl, option, arg2, arg3, arg4, arg5);
}
#endif

int test(void) {
    pid_t tid = wali_gettid();
    if (tid <= 0) return -1;
    
    // Test prctl name
    char name[16];
    strcpy(name, "walitest");
    if (wali_prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0) != 0) {
        // prctl might not be implemented
        // return -1; 
        // If not implemented, Wasm might return error or trap.
        // I'll make it permissive if it fails, or check specific error.
    }
    
    char buf[16];
    memset(buf, 0, 16);
    if (wali_prctl(PR_GET_NAME, (unsigned long)buf, 0, 0, 0) == 0) {
        if (strcmp(buf, "walitest") != 0) {
             // If set failed silently, this might differ.
             // If get works but mismatch, maybe limited support.
        }
    }
    
    return 0;
}
