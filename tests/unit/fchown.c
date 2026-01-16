// CMD: setup="" args="" cleanup=""

#include "wali_start.c"

#define TEST_FILE "/tmp/fchown_test.txt"

#ifdef WALI_TEST_WRAPPER
#include <sys/stat.h>
#include <fcntl.h>
int test_setup(int argc, char **argv) {
    int fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) return -1;
    close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    unlink(TEST_FILE);
    return 0;
}
#endif

int test(void) {
    // 1. Open the file to get a valid fd
    int fd = wali_syscall_open(TEST_FILE, O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);

    // 2. Test Safe No-op (-1, -1)
    TEST_ASSERT_EQ(wali_syscall_fchown(fd, -1, -1), 0);

    // 3. Test Invalid FD
    TEST_ASSERT(wali_syscall_fchown(-1, -1, -1) < 0);

    // 4. Test on a pipe (Implementation defined or error)
    // Skipped for now.
    
    // 5. Test valid change (self)
    int uid = getuid();
    int gid = getgid();
    TEST_ASSERT_EQ(wali_syscall_fchown(fd, uid, gid), 0);

    wali_syscall_close(fd);
    return 0;
}
