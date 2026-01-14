// CMD: setup="" args=""

#define _GNU_SOURCE
#include "wali_start.c"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_mmap")))
long long __imported_wali_mmap(void *addr, size_t length, int prot, int flags, int fd, long long offset);
__attribute__((__import_module__("wali"), __import_name__("SYS_munmap")))
long long __imported_wali_munmap(void *addr, size_t length);
__attribute__((__import_module__("wali"), __import_name__("SYS_madvise")))
long long __imported_wali_madvise(void *addr, size_t length, int advice);

void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    long long res = __imported_wali_mmap(addr, length, prot, flags, fd, (long long)offset);
    if (res < 0 && res > -4096) return MAP_FAILED;
    return (void *)(long)res;
}
int wali_munmap(void *addr, size_t length) { return (int)__imported_wali_munmap(addr, length); }
int wali_madvise(void *addr, size_t length, int advice) { return (int)__imported_wali_madvise(addr, length, advice); }
#else
#include <sys/syscall.h>
#include <sys/mman.h>
void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return mmap(addr, length, prot, flags, fd, offset);
}
int wali_munmap(void *addr, size_t length) { return munmap(addr, length); }
int wali_madvise(void *addr, size_t length, int advice) { return madvise(addr, length, advice); }
#endif

int test(void) {
    size_t size = 4096;
    void *ptr = wali_mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) return -1;
    
    // Just check it doesn't fail
    if (wali_madvise(ptr, size, MADV_NORMAL) != 0) return -1;
    if (wali_madvise(ptr, size, MADV_SEQUENTIAL) != 0) return -1;
    
    wali_munmap(ptr, size);
    return 0;
}
