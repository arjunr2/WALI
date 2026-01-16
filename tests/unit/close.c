// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
#include <errno.h>

#define TEST_FILE "/tmp/wali_close_test"

#ifdef WALI_TEST_WRAPPER
#include <fcntl.h>
int test_setup(int argc, char **argv) {
    // Create test file
    int fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    unlink(TEST_FILE);
    return 0;
}
#endif

int test(void) {
    TEST_LOG("Opening " TEST_FILE);
    int fd = wali_syscall_open(TEST_FILE, O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);
    
    TEST_LOG("Closing valid fd");
    TEST_ASSERT_EQ(wali_syscall_close(fd), 0);
    
    TEST_LOG("Closing already closed fd (should fail)");
    long ret = wali_syscall_close(fd);
    TEST_ASSERT(ret < 0);
    
    // Check errno? In WALI, we might check negative return.
    // Native syscall returns -1, check errno EBADF.
    // Our wrapper: native returns -1 (if using `syscall` wrapper). wrapper is `syscall(...)`.
    // Wait, `wali_syscall_close` returns `long`. 
    // Native `syscall` returns -1 on error.
    // Wasm `__imported` returns whatever host returns.
    // Usually negative errno or -1.
    // Let's just assert failure for now.

    return 0;
}
