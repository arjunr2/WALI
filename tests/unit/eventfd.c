// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
// #include <unistd.h>
// #include <fcntl.h>
// #include <stdint.h>
// #include <string.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
WALI_IMPORT("SYS_eventfd") long wali_syscall_eventfd(unsigned int initval);
WALI_IMPORT("SYS_eventfd2") long wali_syscall_eventfd2(unsigned int initval, int flags);

int wali_eventfd(unsigned int initval) { return (int)wali_syscall_eventfd(initval); }
int wali_eventfd2(unsigned int initval, int flags) { return (int)wali_syscall_eventfd2(initval, flags); }
#else
#include <sys/syscall.h>
int wali_eventfd(unsigned int initval) { return syscall(SYS_eventfd, initval); }
int wali_eventfd2(unsigned int initval, int flags) { return syscall(SYS_eventfd2, initval, flags); }
#endif

#ifndef EFD_CLOEXEC
#define EFD_CLOEXEC 02000000
#endif
#ifndef EFD_NONBLOCK
#define EFD_NONBLOCK 04000
#endif

int test_eventfd_logic(int efd) {
    TEST_ASSERT(efd >= 0);

    uint64_t val;
    // Should be able to read the 10
    TEST_ASSERT_EQ(wali_syscall_read(efd, &val, sizeof(val)), sizeof(val));
    TEST_ASSERT_EQ(val, 10);

    // Now it's 0. Write 5.
    val = 5;
    TEST_ASSERT_EQ(wali_syscall_write(efd, &val, sizeof(val)), sizeof(val));

    // Read 5.
    TEST_ASSERT_EQ(wali_syscall_read(efd, &val, sizeof(val)), sizeof(val));
    TEST_ASSERT_EQ(val, 5);
    
    wali_syscall_close(efd);
    return 0;
}

int test(void) {
    TEST_LOG("Testing eventfd(10)");
    int efd = wali_eventfd(10);
    if (test_eventfd_logic(efd) != 0) return -1;
    
    TEST_LOG("Testing eventfd2(10, EFD_CLOEXEC|EFD_NONBLOCK)");
    efd = wali_eventfd2(10, EFD_CLOEXEC | EFD_NONBLOCK);
    if (test_eventfd_logic(efd) != 0) return -1;

    return 0;
}

