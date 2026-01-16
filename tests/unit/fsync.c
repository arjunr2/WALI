// CMD: setup="/tmp/fsync_file" args="/tmp/fsync_file" cleanup="/tmp/fsync_file"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 1) return -1;
    int fd = open(argv[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 1) return -1;
    unlink(argv[0]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_fsync")))
long long __imported_wali_fsync(int fd);

int wali_fsync(int fd) { return (int)__imported_wali_fsync(fd); }
#else
#include <sys/syscall.h>
int wali_fsync(int fd) { return syscall(SYS_fsync, fd); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    
    int fd = open(fname, O_WRONLY);
    if (fd < 0) return -1;
    
    write(fd, "data", 4);
    
    if (wali_fsync(fd) != 0) return -1;
    
    close(fd);
    return 0;
}
