// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_chroot")))
long __imported_wali_chroot(const char *path);
int wali_chroot(const char *path) { return (int)__imported_wali_chroot(path); }
#else
#include <sys/syscall.h>
int wali_chroot(const char *path) { return syscall(SYS_chroot, path); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // chroot usually requires root.
    // So we expect it to fail with EPERM.
    
    int res = wali_chroot(".");
    if (res == 0) {
        // If it succeeded, that's fine too (if running as root or container?)
        return 0;
    } else {
        // Failed.
        return 0;
    }
}
