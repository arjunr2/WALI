// CMD: setup="" args="" cleanup=""

#include "wali_start.c"

#include <fcntl.h>

#define FILE_OK "/tmp/access_ok.txt"
#define FILE_RW "/tmp/access_rw.txt"
#define FILE_X  "/tmp/access_x.txt"
#define FILE_RO "/tmp/access_ro.txt"
#define FILE_MISSING "/tmp/access_missing.txt"

#ifdef WALI_TEST_WRAPPER
#include <sys/stat.h>
int test_setup(int argc, char **argv) {
    int fd;
    // Cleanup first
    unlink(FILE_OK); unlink(FILE_RW); unlink(FILE_X); unlink(FILE_RO);

    // Create files
    fd = open(FILE_OK, O_WRONLY | O_CREAT | O_TRUNC, 0644); close(fd);
    fd = open(FILE_RW, O_WRONLY | O_CREAT | O_TRUNC, 0666); close(fd); // rw-rw-rw-
    fd = open(FILE_X,  O_WRONLY | O_CREAT | O_TRUNC, 0755); close(fd); // rwxr-xr-x
    fd = open(FILE_RO, O_WRONLY | O_CREAT | O_TRUNC, 0444); close(fd); // r--r--r--
    
    return 0;
}
int test_cleanup(int argc, char **argv) {
    unlink(FILE_OK);
    unlink(FILE_RW);
    unlink(FILE_X);
    unlink(FILE_RO);
    return 0;
}
#endif

int test(void) {
    // 1. Exist
    TEST_ASSERT_EQ(wali_syscall_access(FILE_OK, F_OK), 0);
    TEST_ASSERT_EQ(wali_syscall_access(FILE_OK, R_OK), 0);

    // 2. Fail (Missing)
    TEST_ASSERT(wali_syscall_access(FILE_MISSING, F_OK) < 0);

    // 3. Read/Write
    TEST_ASSERT_EQ(wali_syscall_access(FILE_RW, R_OK), 0);
    TEST_ASSERT_EQ(wali_syscall_access(FILE_RW, W_OK), 0);

    // 4. Exec
    TEST_ASSERT_EQ(wali_syscall_access(FILE_X, X_OK), 0);

    // 5. Read Only (No Write)
    // Note: If running as root (some CI envs), W_OK might succeed even for RO files.
    // We assume non-root for strict check, or just check existence.
    // For now, let's keep the logic strict and see if it passes.
    if (getuid() != 0) {
        TEST_ASSERT(wali_syscall_access(FILE_RO, W_OK) < 0);
    }
    TEST_ASSERT_EQ(wali_syscall_access(FILE_RO, R_OK), 0);
    
    return 0;
}
