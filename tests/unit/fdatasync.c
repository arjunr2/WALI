// CMD: setup="create /tmp/fdatasync_file" args="/tmp/fdatasync_file" cleanup="remove /tmp/fdatasync_file"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_fdatasync")))
long long __imported_wali_fdatasync(int fd);

int wali_fdatasync(int fd) { return (int)__imported_wali_fdatasync(fd); }
#else
#include <sys/syscall.h>
int wali_fdatasync(int fd) { return syscall(SYS_fdatasync, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    
    int fd = open(fname, O_WRONLY);
    if (fd < 0) return -1;
    
    write(fd, "data", 4);
    
    if (wali_fdatasync(fd) != 0) return -1;
    
    close(fd);
    return 0;
}
