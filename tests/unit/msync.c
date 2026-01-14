// CMD: setup="create_file /tmp/msync_file 8192" args="/tmp/msync_file"

#define _GNU_SOURCE
#include "wali_start.c"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 3) return 0;
    const char *fname = argv[1];
    int size = atoi(argv[2]);
    int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        ftruncate(fd, size);
        char buf[1024];
        memset(buf, 'A', 1024);
        for(int i=0; i < (size+1023)/1024; i++) {
            write(fd, buf, (size - i*1024) > 1024 ? 1024 : (size - i*1024));
        }
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
__attribute__((__import_module__("wali"), __import_name__("SYS_msync")))
long long __imported_wali_msync(void *addr, size_t length, int flags);

void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    long long res = __imported_wali_mmap(addr, length, prot, flags, fd, (long long)offset);
    if (res < 0 && res > -4096) return MAP_FAILED;
    return (void *)(long)res;
}
int wali_munmap(void *addr, size_t length) { return (int)__imported_wali_munmap(addr, length); }
int wali_msync(void *addr, size_t length, int flags) { return (int)__imported_wali_msync(addr, length, flags); }

#else
#include <sys/syscall.h>
#include <sys/mman.h>
void *wali_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return mmap(addr, length, prot, flags, fd, offset);
}
int wali_munmap(void *addr, size_t length) { return munmap(addr, length); }
int wali_msync(void *addr, size_t length, int flags) { return msync(addr, length, flags); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    
    int fd = open(fname, O_RDWR);
    if (fd < 0) return -1;
    
    size_t size = 4096;
    void *ptr = wali_mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) return -1;
    
    ((char*)ptr)[0] = 'Z';
    
    if (wali_msync(ptr, size, MS_SYNC) != 0) return -1;
    
    wali_munmap(ptr, size);
    
    // Check file content via read
    char buf[1];
    if (pread(fd, buf, 1, 0) != 1) return -1;
    if (buf[0] != 'Z') return -1;
    
    close(fd);
    return 0;
}
