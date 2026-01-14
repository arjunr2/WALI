// CMD: setup="create /tmp/statx_file" args="file /tmp/statx_file"

#include "wali_start.c"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
#include <stdlib.h>
int test_setup(int argc, char **argv) {
    if (argc < 2) return 0;
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd >= 0) close(fd);
    return 0;
}
int test_cleanup(int argc, char **argv) {
    if (argc < 2) return 0;
    unlink(argv[1]);
    return 0;
}
#endif

// struct statx definition might not be in older headers or WASM environment
// We might need to define it or include generic Linux headers.
// WALI usually provides compatible headers or we match the layout.
// For now, let's assuming it's available or we can use a simplified check if not.

#ifndef AT_STATX_SYNC_AS_STAT
#define AT_STATX_SYNC_AS_STAT	0x0000
#endif
#ifndef STATX_ALL
#define STATX_ALL		0x00000fffU
#endif

// We need a definition of struct statx if not present.
// Since we are compiling against WALI headers or standard headers...
// Let's assume <sys/stat.h> has it or we might need <linux/stat.h>.
#include <sys/syscall.h>

// Note: struct statx is complex. If headers don't strictly match, test might fail compilation.
// Linux header removed for WASM compatibility


#ifdef __wasm__
// Import WALI sycall
// We can't easily reproduce struct statx layout here without copying it.
// So for this test, let's just do a basic presence check or use the syscall if we can.
// If the headers are missing, we might need to skip strict struct checks.
__attribute__((__import_module__("wali"), __import_name__("SYS_statx")))
long __imported_wali_statx(int dirfd, const char *pathname, int flags, unsigned int mask, void *statxbuf);
__attribute__((__import_module__("wali"), __import_name__("SYS_open")))
long __imported_wali_open(const char *pathname, int flags, int mode);
__attribute__((__import_module__("wali"), __import_name__("SYS_close")))
long __imported_wali_close(int fd);

int wali_statx(int dirfd, const char *pathname, int flags, unsigned int mask, void *statxbuf) {
    return (int)__imported_wali_statx(dirfd, pathname, flags, mask, statxbuf);
}
int wali_open(const char *pathname, int flags, int mode) { return (int)__imported_wali_open(pathname, flags, mode); }
int wali_close(int fd) { return (int)__imported_wali_close(fd); }
#else
// Native
// On some systems statx might not be wrapped by glibc (older).
// We'll use syscall directly if needed.
#ifndef SYS_statx
#define SYS_statx 332
#endif

int wali_statx(int dirfd, const char *pathname, int flags, unsigned int mask, void *statxbuf) {
    return syscall(SYS_statx, dirfd, pathname, flags, mask, statxbuf);
}
int wali_open(const char *pathname, int flags, int mode) { return syscall(SYS_open, pathname, flags, mode); }
int wali_close(int fd) { return syscall(SYS_close, fd); }
#endif

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    if (argc < 2) return -1;
    
    // Allocate a buffer large enough for statx (256 bytes usually)
    char buf[256]; 
    memset(buf, 0, sizeof(buf));
    
    // We treating it as opaque for now to avoid struct definition issues
    int res = wali_statx(AT_FDCWD, argv[1], 0, STATX_ALL, buf);
    
    // On systems without statx support (older kernel), this might fail with ENOSYS.
    // But WALI claims to support it. Native host might not?
    // If native doesn't support it, the test will fail on native side?
    // We can assume the host is recent enough given WALI requirements.
    
    if (res != 0) return -1;
    
    return 0;
}
