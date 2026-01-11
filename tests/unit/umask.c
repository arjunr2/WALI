// CMD: setup="" args="umask /tmp/um_test_file"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[1]);
    return 0;
}
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_umask")))
long long __imported_wali_umask(int mask);
int wali_umask(int mask) { return (int)__imported_wali_umask(mask); }
#else
#include <sys/syscall.h>
int wali_umask(int mask) { return syscall(SYS_umask, mask); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *fname = argv[1];
    
    // Set umask to 0
    int old = wali_umask(0);
    // Create file with 0666 -> should be 0666
    int fd = open(fname, O_CREAT | O_WRONLY, 0666);
    close(fd);
    
    struct stat st;
    if (stat(fname, &st) != 0) return -1;
    if ((st.st_mode & 0777) != 0666) return -1;
    
    unlink(fname);
    
    // Set umask to 077
    wali_umask(0077);
    fd = open(fname, O_CREAT | O_WRONLY, 0666);
    close(fd);
    
    if (stat(fname, &st) != 0) return -1;
    // 0666 & ~0077 = 0600
    if ((st.st_mode & 0777) != 0600) return -1;
    
    unlink(fname);
    
    return 0;
}
