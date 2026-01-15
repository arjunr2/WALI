// CMD: setup="/tmp/readlinkat_link /tmp/readlinkat_src" args="/tmp/readlinkat_link /tmp/readlinkat_src" cleanup="/tmp/readlinkat_link /tmp/readlinkat_src"

#include "wali_start.c"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *link = argv[0];
    const char *target = argv[1];
    // Create target file
    int fd = open(target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) close(fd);
    // Create symlink
    symlink(target, link);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[0]);
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_readlinkat")))
long __imported_wali_readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz);

ssize_t wali_readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) { 
    return (ssize_t)__imported_wali_readlinkat(dirfd, pathname, buf, bufsiz); 
}

#else
#include <sys/syscall.h>
ssize_t wali_readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) { 
    return syscall(SYS_readlinkat, dirfd, pathname, buf, bufsiz); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *link = argv[0];
    const char *expected_target = argv[1];
    
    char buf[256];
    ssize_t len = wali_readlinkat(AT_FDCWD, link, buf, sizeof(buf));
    if (len < 0) return -1;
    
    buf[len] = '\0';
    if (strcmp(buf, expected_target) != 0) return -1;
    
    return 0;
}
