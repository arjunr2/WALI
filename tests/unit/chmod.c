// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
#include <sys/stat.h>

#define TEST_FILE "/tmp/chmod_test.txt"

#ifdef WALI_TEST_WRAPPER
#include <unistd.h>
#include <fcntl.h>
int test_setup(int argc, char **argv) {
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
    struct stat st;
    
    TEST_LOG("Testing chmod " TEST_FILE " to 0777");
    TEST_ASSERT_EQ(wali_syscall_chmod(TEST_FILE, 0777), 0);
    
    TEST_ASSERT_EQ(wali_syscall_stat(TEST_FILE, &st), 0);
    // Verify mode. Note: 0777 might be masked by umask, but we forced chmod.
    // Usually chmod overrides umask.
    TEST_ASSERT_EQ((st.st_mode & 0777), 0777);
    
    TEST_LOG("Testing fchmod to 0600");
    long fd = wali_syscall_open(TEST_FILE, O_RDONLY, 0);
    TEST_ASSERT_NE(fd, -1);
    
    TEST_ASSERT_EQ(wali_syscall_fchmod(fd, 0600), 0);
    wali_syscall_close(fd);
    
    TEST_ASSERT_EQ(wali_syscall_stat(TEST_FILE, &st), 0);
    TEST_ASSERT_EQ((st.st_mode & 0777), 0600);
    
    return 0;
}
