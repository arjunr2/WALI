// CMD: setup="create /tmp/fcntl_file" args="basic /tmp/fcntl_file"
// CMD: setup="create /tmp/fcntl_fd" args="fd_flags /tmp/fcntl_fd"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

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
__attribute__((__import_module__("wali"), __import_name__("SYS_fcntl")))
long long __imported_wali_fcntl(int fd, int cmd, unsigned long arg);
int wali_fcntl(int fd, int cmd, unsigned long arg) { return (int)__imported_wali_fcntl(fd, cmd, arg); }
#else
#include <sys/syscall.h>
int wali_fcntl(int fd, int cmd, unsigned long arg) { return syscall(SYS_fcntl, fd, cmd, arg); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *mode = argv[0];
    const char *fname = argv[1];
    
    int fd = open(fname, O_RDWR);
    if (fd < 0) return -1;
    
    if (strcmp(mode, "basic") == 0) {
        // F_DUPFD
        int newfd = wali_fcntl(fd, F_DUPFD, 0);
        if (newfd < 0) return -1;
        if (newfd == fd) return -1;
        
        // Check write to newfd
        if (write(newfd, "A", 1) != 1) return -1;
        close(newfd);
        
        // F_GETFL
        int flags = wali_fcntl(fd, F_GETFL, 0);
        if (flags < 0) return -1;
        if ((flags & O_ACCMODE) != O_RDWR) return -1;
        
        // F_SETFL (O_APPEND)
        if (wali_fcntl(fd, F_SETFL, flags | O_APPEND) != 0) return -1;
        
        flags = wali_fcntl(fd, F_GETFL, 0);
        if (!(flags & O_APPEND)) return -1;
    
    } else if (strcmp(mode, "fd_flags") == 0) {
        // F_GETFD
        int flags = wali_fcntl(fd, F_GETFD, 0);
        if (flags < 0) return -1;
        if (flags & FD_CLOEXEC) return -1; // Should be 0 initially
        
        // F_SETFD
        if (wali_fcntl(fd, F_SETFD, flags | FD_CLOEXEC) != 0) return -1;
        
        flags = wali_fcntl(fd, F_GETFD, 0);
        if (!(flags & FD_CLOEXEC)) return -1;
        
        // F_DUPFD_CLOEXEC (linux only usually, check if defined)
        #ifdef F_DUPFD_CLOEXEC
        int fd2 = wali_fcntl(fd, F_DUPFD_CLOEXEC, 0);
        if (fd2 < 0) return -1;
        flags = wali_fcntl(fd2, F_GETFD, 0);
        if (!(flags & FD_CLOEXEC)) return -1;
        close(fd2);
        #endif
    }

    close(fd);
    return 0;
}
