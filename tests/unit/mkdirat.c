// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
#include <fcntl.h>
#include <errno.h>

#define PARENT_DIR "/tmp/mkdirat_parent"
#define SUB_DIR    "subdir"
#define SUB_PATH   "/tmp/mkdirat_parent/subdir"

#ifdef WALI_TEST_WRAPPER
#include <sys/stat.h>
int test_setup(int argc, char **argv) {
    rmdir(SUB_PATH);
    rmdir(PARENT_DIR);
    mkdir(PARENT_DIR, 0755);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    rmdir(SUB_PATH);
    rmdir(PARENT_DIR);
    return 0;
}
#endif

int test(void) {
    int dirfd = wali_syscall_open(PARENT_DIR, O_RDONLY | O_DIRECTORY, 0);
    TEST_ASSERT(dirfd >= 0);

    // Test 1: Simple mkdirat (Success)
    TEST_ASSERT_EQ(wali_syscall_mkdirat(dirfd, SUB_DIR, 0755), 0);

    // Test 2: EEXIST
    TEST_ASSERT(wali_syscall_mkdirat(dirfd, SUB_DIR, 0755) < 0);

    // Test 3: Invalid FD
    TEST_ASSERT(wali_syscall_mkdirat(-100, "foo", 0755) < 0);

    wali_syscall_close(dirfd);

    // Verify created dir (using syscall_open to check existence)
    int subfd = wali_syscall_open(SUB_PATH, O_RDONLY | O_DIRECTORY, 0);
    TEST_ASSERT(subfd >= 0);
    wali_syscall_close(subfd);

    return 0;
}
