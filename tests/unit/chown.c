// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
#include <fcntl.h>

#define TEST_FILE "/tmp/chown_test.txt"
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

#ifdef WALI_TEST_WRAPPER
#include <unistd.h>
int test_setup(int argc, char **argv) {
    int fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    unlink(TEST_FILE);
    return 0;
}
#endif

int test(void) {
    TEST_LOG("Testing chown no-op (-1, -1)");
    TEST_ASSERT_EQ(wali_syscall_chown(TEST_FILE, -1, -1), 0);
    return 0;
}
