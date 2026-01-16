// CMD: setup="/tmp/ft_file" args="ftruncate /tmp/ft_file" cleanup="/tmp/ft_file"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, "12345", 5);
        close(fd);
    }
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_ftruncate")))
long long __imported_wali_ftruncate(int fd, long long length);
int wali_ftruncate(int fd, off_t length) { return (int)__imported_wali_ftruncate(fd, (long long)length); }
#else
#include <sys/syscall.h>
int wali_ftruncate(int fd, off_t length) { return syscall(SYS_ftruncate, fd, length); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    
    int fd = open(fname, O_RDWR);
    if (fd < 0) return -1;
    
    // Initial size 5. Truncate to 2.
    if (wali_ftruncate(fd, 2) != 0) return -1;
    
    struct stat st;
    if (fstat(fd, &st) != 0) return -1;
    if (st.st_size != 2) return -1;
    
    // Extent to 10
    if (wali_ftruncate(fd, 10) != 0) return -1;
    if (fstat(fd, &st) != 0) return -1;
    if (st.st_size != 10) return -1;
    
    return 0;
}
