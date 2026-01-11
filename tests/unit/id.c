// CMD: setup="" args=""

#include "wali_start.c"
#include <unistd.h>
#include <sys/types.h>

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getuid")))
long long __imported_wali_getuid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_getgid")))
long long __imported_wali_getgid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_geteuid")))
long long __imported_wali_geteuid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_getegid")))
long long __imported_wali_getegid(void);
__attribute__((__import_module__("wali"), __import_name__("SYS_getppid")))
long long __imported_wali_getppid(void);

uid_t wali_getuid(void) { return (uid_t)__imported_wali_getuid(); }
gid_t wali_getgid(void) { return (gid_t)__imported_wali_getgid(); }
uid_t wali_geteuid(void) { return (uid_t)__imported_wali_geteuid(); }
gid_t wali_getegid(void) { return (gid_t)__imported_wali_getegid(); }
pid_t wali_getppid(void) { return (pid_t)__imported_wali_getppid(); }

// pid_t wali_getpgrp(void) { return (pid_t)__imported_wali_getpgrp(); }

#else
#include <sys/syscall.h>
uid_t wali_getuid(void) { return syscall(SYS_getuid); }
gid_t wali_getgid(void) { return syscall(SYS_getgid); }
uid_t wali_geteuid(void) { return syscall(SYS_geteuid); }
gid_t wali_getegid(void) { return syscall(SYS_getegid); }
pid_t wali_getppid(void) { return syscall(SYS_getppid); }
// pid_t wali_getpgrp(void) { return syscall(SYS_getpgrp); }
#endif

int test(void) {
    if (wali_getuid() == (uid_t)-1) return -1;
    if (wali_getgid() == (gid_t)-1) return -1;
    if (wali_geteuid() == (uid_t)-1) return -1;
    if (wali_getegid() == (gid_t)-1) return -1;
    // ppid/pgrp might be 0 in init but typically >0
    wali_getppid(); 
    // wali_getpgrp();
    return 0;
}
