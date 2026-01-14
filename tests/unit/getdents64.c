// CMD: setup="create_dir /tmp/d1" args="ls /tmp/d1"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *dname = argv[1]; 
    mkdir(dname, 0755);
    char buf[256];
    
    snprintf(buf, sizeof(buf), "%s/f1", dname);
    int fd = open(buf, O_WRONLY | O_CREAT, 0644);
    if (fd >= 0) close(fd);
    
    snprintf(buf, sizeof(buf), "%s/f2", dname);
    fd = open(buf, O_WRONLY | O_CREAT, 0644);
    if (fd >= 0) close(fd);
    
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    const char *dname = argv[1];
    char buf[256];
    snprintf(buf, sizeof(buf), "%s/f1", dname);
    unlink(buf);
    snprintf(buf, sizeof(buf), "%s/f2", dname);
    unlink(buf);
    rmdir(dname);
    return 0;
}
#endif

/*
struct linux_dirent64 {
    uint64_t        d_ino;
    int64_t         d_off;
    unsigned short  d_reclen;
    unsigned char   d_type;
    char            d_name[];
};
*/
struct linux_dirent64 {
    uint64_t        d_ino; // 8
    int64_t         d_off; // 8
    unsigned short  d_reclen; // 2
    unsigned char   d_type; // 1
    char            d_name[];
}; // Packed? No, usually aligned. But let's work with offsets manually if needed.
// Actually standard struct layout rules apply.

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getdents64")))
long long __imported_wali_getdents64(int fd, struct linux_dirent64 *dirp, int count);
int wali_getdents64(int fd, void *dirp, int count) { return (int)__imported_wali_getdents64(fd, dirp, count); }
#else
#include <sys/syscall.h>
int wali_getdents64(int fd, void *dirp, int count) { return syscall(SYS_getdents64, fd, dirp, count); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    const char *dname = argv[1];
    
    int fd = open(dname, O_RDONLY | O_DIRECTORY);
    if (fd < 0) return -1;
    
    char buf[1024];
    int nread;
    int f1_found = 0;
    int f2_found = 0;
    
    while ((nread = wali_getdents64(fd, buf, sizeof(buf))) > 0) {
        int bpos = 0;
        while (bpos < nread) {
            struct linux_dirent64 *d = (struct linux_dirent64 *)(buf + bpos);
            if (strcmp(d->d_name, "f1") == 0) f1_found = 1;
            if (strcmp(d->d_name, "f2") == 0) f2_found = 1;
            bpos += d->d_reclen;
        }
    }
    
    close(fd);
    
    if (!f1_found || !f2_found) return -1;
    return 0;
}
