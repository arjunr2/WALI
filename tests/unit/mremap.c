// CMD: args="grow_maymove"
// CMD: args="shrink"

#define _GNU_SOURCE
#include "wali_start.c"
#include <sys/mman.h>
#include <string.h>

#ifndef MREMAP_MAYMOVE
#define MREMAP_MAYMOVE 1
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_mmap")))
long long __imported_wali_mmap(void *addr, size_t length, int prot, int flags, int fd, long long offset);
__attribute__((__import_module__("wali"), __import_name__("SYS_munmap")))
long long __imported_wali_munmap(void *addr, size_t length);
__attribute__((__import_module__("wali"), __import_name__("SYS_mremap")))
long long __imported_wali_mremap(void *old_address, size_t old_size, size_t new_size, int flags, void *new_address);

void *wali_mmap(void *a, size_t l, int p, int f, int fd, off_t o) {
    long long r = __imported_wali_mmap(a, l, p, f, fd, (long long)o);
    if (r < 0 && r > -4096) return MAP_FAILED;
    return (void *)(long)r;
}
int wali_munmap(void *a, size_t l) { return (int)__imported_wali_munmap(a, l); }
void *wali_mremap(void *old, size_t os, size_t ns, int flags, void *na) {
    long long r = __imported_wali_mremap(old, os, ns, flags, na);
    if (r < 0 && r > -4096) return MAP_FAILED;
    return (void *)(long)r;
}
#else
#include <sys/syscall.h>
void *wali_mmap(void *a, size_t l, int p, int f, int fd, off_t o) { return mmap(a, l, p, f, fd, o); }
int wali_munmap(void *a, size_t l) { return munmap(a, l); }
void *wali_mremap(void *old, size_t os, size_t ns, int flags, void *na) { return mremap(old, os, ns, flags, na); }
#endif

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *mode = (argc > 1) ? argv[1] : "grow_maymove";

    size_t old_size = 4096;
    void *ptr = wali_mmap(NULL, old_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) return -1;
    ((char *)ptr)[0] = 'X';

    size_t new_size = !strcmp(mode, "shrink") ? 4096 / 2 : 8192;
    void *new_ptr = wali_mremap(ptr, old_size, new_size, MREMAP_MAYMOVE, NULL);
    if (new_ptr == MAP_FAILED) { wali_munmap(ptr, old_size); return -1; }

    int ret = (((char *)new_ptr)[0] == 'X') ? 0 : -1;
    wali_munmap(new_ptr, new_size);
    return ret;
}
