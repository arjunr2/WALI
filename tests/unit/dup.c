// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>

#include <string.h>
#include <fcntl.h>

#define TEST_FILE "/tmp/dup_test.txt"
#define TEST_CONTENT "DUP_TEST"

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
#include <stdio.h>

int test_setup(int argc, char **argv) {
    int fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    write(fd, TEST_CONTENT, strlen(TEST_CONTENT));
    close(fd);
    return 0;
}

int test_cleanup(int argc, char **argv) {
    unlink(TEST_FILE);
    return 0;
}
#endif

int test(void) {
    int fd = wali_syscall_open(TEST_FILE, O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);
    
    // Test dup
    int fd2 = wali_syscall_dup(fd);
    if (fd2 < 0) { 
        wali_syscall_close(fd); 
        TEST_FAIL("dup failed"); 
    }
    
    if (fd2 == fd) { 
        wali_syscall_close(fd); 
        wali_syscall_close(fd2); 
        TEST_FAIL("dup returned same fd?"); 
    }
    
    char buf[16];
    memset(buf, 0, sizeof(buf));
    int len = strlen(TEST_CONTENT);
    if (wali_syscall_read(fd2, buf, len) != len) {
         wali_syscall_close(fd); wali_syscall_close(fd2); 
         TEST_FAIL("read failed");
    }
    
    if (strcmp(buf, TEST_CONTENT) != 0) {
        wali_syscall_close(fd); wali_syscall_close(fd2); 
        TEST_FAIL("Content mismatch");
    }
    
    wali_syscall_close(fd2);
    wali_syscall_close(fd);
    return 0;
}

