// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>

#include <string.h>
#include <fcntl.h>

#define TEST_FILE "/tmp/dup2_test.txt"
#define TEST_CONTENT "DUP2_TEST"

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

#ifdef __wasm__
WALI_IMPORT("SYS_dup2") long wali_syscall_dup2(int oldfd, int newfd);
#else
#include <sys/syscall.h>
long wali_syscall_dup2(int oldfd, int newfd) { return syscall(SYS_dup2, oldfd, newfd); }
#endif

int test(void) {
    int fd = wali_syscall_open(TEST_FILE, O_RDONLY, 0);
    TEST_ASSERT(fd >= 0);
    
    // Test dup2
    int target_fd = 10;
    // Ensure target closed
    wali_syscall_close(target_fd); 
    
    // Call dup2 with explicit call
    long res = wali_syscall_dup2(fd, target_fd);
    
    // Check result
    if (res != target_fd) { 
        wali_syscall_close(fd); 
        TEST_FAIL("dup2 failed or returned wrong fd"); 
    }
    
    char buf[16];
    memset(buf, 0, sizeof(buf));
    int len = strlen(TEST_CONTENT);
    if (wali_syscall_read(target_fd, buf, len) != len) {
        wali_syscall_close(fd); wali_syscall_close(target_fd); 
        TEST_FAIL("read failed from new fd");
    }
    
    if (strcmp(buf, TEST_CONTENT) != 0) { 
        wali_syscall_close(fd); wali_syscall_close(target_fd); 
        TEST_FAIL("Content mismatch");
    }
    
    wali_syscall_close(fd);
    wali_syscall_close(target_fd);
    
    return 0;
}

