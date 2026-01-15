// CMD: args="basic"

#include "wali_start.c"
#include <sys/mman.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_mmap")))
long __imported_wali_mmap(void *addr, size_t length, int prot, int flags, int fd, long offset);
__attribute__((__import_module__("wali"), __import_name__("SYS_munmap")))
long __imported_wali_munmap(void *addr, size_t length);

void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, long offset) { 
    return (void*)__imported_wali_mmap(addr, length, prot, flags, fd, offset); 
}
int wali_munmap(void *addr, size_t length) { return (int)__imported_wali_munmap(addr, length); }

#else
#include <sys/syscall.h>
void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, long offset) { 
    return (void*)syscall(SYS_mmap, addr, length, prot, flags, fd, offset); 
}
int wali_munmap(void *addr, size_t length) { return syscall(SYS_munmap, addr, length); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    size_t page_size = 4096;
    
    // Map anonymous memory
    void *ptr = wali_mmap(0, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) return -1;
    
    // Write to it
    memset(ptr, 'B', page_size);
    
    // Unmap it
    if (wali_munmap(ptr, page_size) != 0) return -1;
    
    // Unmap again should fail
    int ret = wali_munmap(ptr, page_size);
    // May or may not fail depending on implementation
    (void)ret;
    
    return 0;
}
