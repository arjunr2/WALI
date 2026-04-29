// CMD: args="set_get_name"
// CMD: args="bad_option"

#include "wali_start.c"
#include <sys/prctl.h>
#include <string.h>

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_prctl")))
long __imported_wali_prctl(int option, unsigned long a2, unsigned long a3, unsigned long a4, unsigned long a5);
int wali_prctl(int o, unsigned long a, unsigned long b, unsigned long c, unsigned long d) {
    return (int)__imported_wali_prctl(o, a, b, c, d);
}
#else
#include <sys/syscall.h>
int wali_prctl(int o, unsigned long a, unsigned long b, unsigned long c, unsigned long d) {
    return syscall(SYS_prctl, o, a, b, c, d);
}
#endif

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *mode = (argc > 1) ? argv[1] : "set_get_name";

    if (!strcmp(mode, "set_get_name")) {
        // Both PR_SET_NAME and PR_GET_NAME take a 16-byte buffer pointer.
        char name[16]; strcpy(name, "walitest");
        if (wali_prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0) != 0) return -1;
        char buf[16] = {0};
        if (wali_prctl(PR_GET_NAME, (unsigned long)buf, 0, 0, 0) != 0) return -1;
        return (strcmp(buf, "walitest") == 0) ? 0 : -1;
    }
    if (!strcmp(mode, "bad_option")) {
        long r = wali_prctl(99999, 0, 0, 0, 0);
        return (r < 0) ? 0 : -1;
    }
    return -1;
}
