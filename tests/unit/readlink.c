// CMD: setup="create /tmp/target_rl link /tmp/target_rl /tmp/link_rl" args="/tmp/link_rl" cleanup="remove /tmp/target_rl /tmp/link_rl"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *target = "/tmp/target_rl";
    const char *linkpath = argv[1];
    
    int fd = open(target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, "DATA", 4);
        close(fd);
    }
    symlink(target, linkpath);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    unlink("/tmp/target_rl");
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_readlink")))
long long __imported_wali_readlink(const char *pathname, char *buf, size_t bufsiz);

ssize_t wali_readlink(const char *pathname, char *buf, size_t bufsiz) { return (ssize_t)__imported_wali_readlink(pathname, buf, bufsiz); }
#else
#include <sys/syscall.h>
ssize_t wali_readlink(const char *pathname, char *buf, size_t bufsiz) { return syscall(SYS_readlink, pathname, buf, bufsiz); }
#endif

int test(void) {
  if (test_init_args() != 0) return -1;
  const char *linkpath = argv[1];
  
  char buf[64];
  ssize_t len = wali_readlink(linkpath, buf, 64);
  if (len < 0) return -1;
  buf[len] = '\0';
  
  if (strcmp(buf, "/tmp/target_rl") != 0) return -1;
  
  return 0;
}
