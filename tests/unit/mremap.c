// CMD: setup="create_file /tmp/mremap_file 4096" args="/tmp/mremap_file"

#define _GNU_SOURCE
#include "wali_start.c"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#ifndef MREMAP_MAYMOVE
#define MREMAP_MAYMOVE 1
#endif

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 3) return 0;
    const char *fname = argv[1];
    int size = atoi(argv[2]);
    int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        ftruncate(fd, size);
        close(fd);
    }
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_mmap")))
long long __imported_wali_mmap(void *addr, size_t length, int prot, int flags, int fd, long long offset);
__attribute__((__import_module__("wali"), __import_name__("SYS_munmap")))
long long __imported_wali_munmap(void *addr, size_t length);
__attribute__((__import_module__("wali"), __import_name__("SYS_mremap")))
long long __imported_wali_mremap(void *old_address, size_t old_size, size_t new_size, int flags, void *new_address);

void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    long long res = __imported_wali_mmap(addr, length, prot, flags, fd, (long long)offset);
    if (res < 0 && res > -4096) return MAP_FAILED;
    return (void *)(long)res;
}
int wali_munmap(void *addr, size_t length) { return (int)__imported_wali_munmap(addr, length); }
void *wali_mremap(void *old_address, size_t old_size, size_t new_size, int flags, void *new_address) {
    long long res = __imported_wali_mremap(old_address, old_size, new_size, flags, new_address);
    if (res < 0 && res > -4096) return MAP_FAILED;
    return (void *)(long)res;
}
#else
#include <sys/syscall.h>
#include <sys/mman.h>
void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return mmap(addr, length, prot, flags, fd, offset);
}
int wali_munmap(void *addr, size_t length) { return munmap(addr, length); }
void *wali_mremap(void *old_address, size_t old_size, size_t new_size, int flags, void *new_address) {
    return mremap(old_address, old_size, new_size, flags, new_address);
}
#endif

int test(void) {
    size_t old_size = 4096;
    void *ptr = wali_mmap(NULL, old_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) return -1;
    
    // Write something
    ((char*)ptr)[0] = 'X';
    
    // Expand
    size_t new_size = 8192;
    void *new_ptr = wali_mremap(ptr, old_size, new_size, MREMAP_MAYMOVE, NULL);
    if (new_ptr == MAP_FAILED) return -1;
    
    // Check data preserved
    if (((char*)new_ptr)[0] != 'X') return -1;
    
    // Check access to new area
    ((char*)new_ptr)[5000] = 'Y';
    
    wali_munmap(new_ptr, new_size);
    return 0;
}
