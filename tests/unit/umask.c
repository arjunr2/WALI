// CMD: setup="" args="umask /tmp/um_file_1 /tmp/um_file_2"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) {
    if (argc < 3) return 0;
    
    // File 1 -> 0666, umask 0 => 0666
    struct stat st;
    if (stat(argv[1], &st) != 0) {
         fprintf(stderr, "[Native Hook] File %s not created\n", argv[1]);
         return 1;
    }
    if ((st.st_mode & 0777) != 0666) {
        fprintf(stderr, "[Native Hook] File %s mode mismatch. Expected 0666, got %o\n", argv[1], st.st_mode & 0777);
        unlink(argv[1]);
        unlink(argv[2]);
        return 1;
    }
    unlink(argv[1]);
    
    // File 2 -> 0666, umask 0077 => 0600
    if (stat(argv[2], &st) != 0) {
         fprintf(stderr, "[Native Hook] File %s not created\n", argv[2]);
         return 1;
    }
    if ((st.st_mode & 0777) != 0600) {
        fprintf(stderr, "[Native Hook] File %s mode mismatch. Expected 0600, got %o\n", argv[2], st.st_mode & 0777);
        unlink(argv[2]);
        return 1;
    }
    unlink(argv[2]);

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
    if (argc < 3) return -1;
    
    // Set umask to 0
    wali_umask(0);
    // Create file 1 with 0666 -> should be 0666
    int fd = open(argv[1], O_CREAT | O_WRONLY, 0666);
    close(fd);
    
    // Set umask to 077
    wali_umask(0077);
    // Create file 2 with 0666 -> should be 0600
    fd = open(argv[2], O_CREAT | O_WRONLY, 0666);
    close(fd);
    
    return 0;
}
