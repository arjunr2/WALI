// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_pipe2")))
long __imported_wali_pipe2(int *pipefd, int flags);
__attribute__((__import_module__("wali"), __import_name__("SYS_fcntl")))
long __imported_wali_fcntl(int fd, int cmd, int arg);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_pipe2(int *pipefd, int flags) { return (int)__imported_wali_pipe2(pipefd, flags); }
int wali_fcntl(int fd, int cmd, int arg) { return (int)__imported_wali_fcntl(fd, cmd, arg); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
#else
#include <sys/syscall.h>
int wali_pipe2(int *pipefd, int flags) { return syscall(SYS_pipe2, pipefd, flags); }
int wali_fcntl(int fd, int cmd, int arg) { return syscall(SYS_fcntl, fd, cmd, arg); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

#ifndef O_CLOEXEC
#define O_CLOEXEC 02000000
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK 04000
#endif
#ifndef FD_CLOEXEC
#define FD_CLOEXEC 1
#endif
#ifndef F_GETFL
#define F_GETFL 3
#endif
#ifndef F_GETFD
#define F_GETFD 1
#endif

int test(void) {
    if (test_init_args() != 0) return -1;

    int pipefd[2];
    int res = wali_pipe2(pipefd, O_CLOEXEC | O_NONBLOCK);
    if (res != 0) return -1;

    // Check O_CLOEXEC on read end
    int flags = wali_fcntl(pipefd[0], F_GETFD, 0);
    if (!(flags & FD_CLOEXEC)) return -1;

    // Check O_NONBLOCK on read end
    flags = wali_fcntl(pipefd[0], F_GETFL, 0);
    if (!(flags & O_NONBLOCK)) return -1;
    
    // Check write end too if we want, typically they share or both set
    
    wali_close(pipefd[0]);
    wali_close(pipefd[1]);

    return 0;
}
