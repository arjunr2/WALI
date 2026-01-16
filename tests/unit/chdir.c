// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
#include <string.h>

#define TEST_DIR "/tmp/sub_chdir"

#ifdef WALI_TEST_WRAPPER
#include <unistd.h>
int test_setup(int argc, char **argv) {
    rmdir(TEST_DIR);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    rmdir(TEST_DIR);
    return 0;
}
#endif

int test(void) {
    char cwd1[512];
    char cwd2[512];
    
    TEST_LOG("Getting initial CWD");
    TEST_ASSERT(wali_syscall_getcwd(cwd1, sizeof(cwd1)) > 0);
    
    TEST_LOG("Creating directory " TEST_DIR);
    TEST_ASSERT_EQ(wali_syscall_mkdir(TEST_DIR, 0755), 0);
    
    TEST_LOG("Changing directory to " TEST_DIR);
    TEST_ASSERT_EQ(wali_syscall_chdir(TEST_DIR), 0);
    
    TEST_LOG("Verifying new CWD");
    TEST_ASSERT(wali_syscall_getcwd(cwd2, sizeof(cwd2)) > 0);
    
    // Verify cwd2 contains TEST_DIR
    TEST_ASSERT(strstr(cwd2, "sub_chdir") != NULL);
    
    TEST_LOG("Going back up");
    TEST_ASSERT_EQ(wali_syscall_chdir(".."), 0);
    
    TEST_LOG("Cleaning up");
    TEST_ASSERT_EQ(wali_syscall_rmdir(TEST_DIR), 0);
    
    return 0;
}
