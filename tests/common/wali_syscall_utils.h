/*
 * wali_syscall_utils.h
 * 
 * Centralized syscall imports and wrapper definitions for WALI unit tests.
 * This file drastically reduces boilerplate in individual test files.
 */

#ifndef WALI_SYSCALL_UTILS_H
#define WALI_SYSCALL_UTILS_H

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>

#ifdef __wasm__

#include <sys/stat.h>
/* -------------------------------------------------------------------------
 * Wasm Imports
 * ------------------------------------------------------------------------- */
#define WALI_IMPORT(name) __attribute__((__import_module__("wali"), __import_name__(name)))

// File Operations
WALI_IMPORT("SYS_open") long wali_syscall_open(const char *pathname, int flags, int mode);
WALI_IMPORT("SYS_close") long wali_syscall_close(int fd);
WALI_IMPORT("SYS_read") long wali_syscall_read(int fd, void *buf, size_t count);
WALI_IMPORT("SYS_write") long wali_syscall_write(int fd, const void *buf, size_t count);
WALI_IMPORT("SYS_lseek") long wali_syscall_lseek(int fd, long offset, int whence);
WALI_IMPORT("SYS_mkdirat") long wali_syscall_mkdirat(int dirfd, const char *pathname, int mode);
WALI_IMPORT("SYS_mkdir") long wali_syscall_mkdir(const char *pathname, int mode);
WALI_IMPORT("SYS_unlink") long wali_syscall_unlink(const char *pathname);
WALI_IMPORT("SYS_rmdir") long wali_syscall_rmdir(const char *pathname);
WALI_IMPORT("SYS_chdir") long wali_syscall_chdir(const char *pathname);
WALI_IMPORT("SYS_getcwd") long long wali_syscall_getcwd(char *buf, size_t size);
WALI_IMPORT("SYS_fchown") long wali_syscall_fchown(int fd, int owner, int group);
WALI_IMPORT("SYS_fchownat") long wali_syscall_fchownat(int dirfd, const char *pathname, int owner, int group, int flags);
WALI_IMPORT("SYS_chown") long wali_syscall_chown(const char *pathname, int owner, int group);
WALI_IMPORT("SYS_chmod") long wali_syscall_chmod(const char *pathname, int mode);
WALI_IMPORT("SYS_fchmod") long wali_syscall_fchmod(int fd, int mode);
WALI_IMPORT("SYS_stat") long wali_syscall_stat(const char *pathname, struct stat *statbuf);
WALI_IMPORT("SYS_pipe2") long wali_syscall_pipe2(int pipefd[2], int flags);
WALI_IMPORT("SYS_dup") long wali_syscall_dup(int fd);

// Network
WALI_IMPORT("SYS_socket") long wali_syscall_socket(int domain, int type, int protocol);
WALI_IMPORT("SYS_bind") long wali_syscall_bind(int sockfd, const void *addr, int addrlen);
WALI_IMPORT("SYS_listen") long wali_syscall_listen(int sockfd, int backlog);
WALI_IMPORT("SYS_accept") long wali_syscall_accept(int sockfd, void *addr, void *addrlen);
WALI_IMPORT("SYS_accept4") long wali_syscall_accept4(int sockfd, void *addr, void *addrlen, int flags);
WALI_IMPORT("SYS_connect") long wali_syscall_connect(int sockfd, const void *addr, int addrlen);
WALI_IMPORT("SYS_getsockname") long wali_syscall_getsockname(int sockfd, void *addr, void *addrlen);

// Process
WALI_IMPORT("SYS_fork") long wali_syscall_fork(void);
WALI_IMPORT("SYS_exit") void wali_syscall_exit(int status);
WALI_IMPORT("SYS_wait4") long wali_syscall_wait4(int pid, int *status, int options, void *rusage);
// WALI_IMPORT("SYS_brk") long wali_syscall_brk(void *addr); // Locals

// Misc
WALI_IMPORT("SYS_fcntl") long wali_syscall_fcntl(int fd, int cmd, long arg);
WALI_IMPORT("SYS_access") long wali_syscall_access(const char *pathname, int mode);
// WALI_IMPORT("SYS_alarm") long wali_syscall_alarm(unsigned int seconds); // Locals
WALI_IMPORT("SYS_rt_sigaction") long wali_syscall_rt_sigaction(int signum, const void *act, void *oldact, size_t sigsetsize);

// Time
WALI_IMPORT("SYS_clock_gettime") long wali_syscall_clock_gettime(clockid_t clock_id, struct timespec *tp);
WALI_IMPORT("SYS_clock_getres") long wali_syscall_clock_getres(clockid_t clock_id, struct timespec *res);
WALI_IMPORT("SYS_clock_nanosleep") long wali_syscall_clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain);

#else // Native

/* -------------------------------------------------------------------------
 * Wrappers (Native Loop)
 * ------------------------------------------------------------------------- */
// Using direct syscall calls to ensure we test the kernel interface, not libc wrappers where possible
// though for some simple ones libc wrappers are unavoidable or identical.

static inline long wali_syscall_fchown(int fd, int owner, int group) { return syscall(SYS_fchown, fd, owner, group); }
static inline long wali_syscall_mkdirat(int dirfd, const char *pathname, int mode) { return syscall(SYS_mkdirat, dirfd, pathname, mode); }
static inline long wali_syscall_mkdir(const char *pathname, int mode) {
#ifdef SYS_mkdir
    return syscall(SYS_mkdir, pathname, mode);
#else
    return syscall(SYS_mkdirat, AT_FDCWD, pathname, mode);
#endif
}
static inline long wali_syscall_rmdir(const char *pathname) {
#ifdef SYS_rmdir
    return syscall(SYS_rmdir, pathname);
#else
    return syscall(SYS_unlinkat, AT_FDCWD, pathname, AT_REMOVEDIR);
#endif
}
static inline long wali_syscall_chdir(const char *pathname) { return syscall(SYS_chdir, pathname); }
static inline long wali_syscall_getcwd(char *buf, unsigned long size) { return syscall(SYS_getcwd, buf, size); }
static inline long wali_syscall_write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
static inline long wali_syscall_read(int fd, void *buf, size_t count) { return syscall(SYS_read, fd, buf, count); }
static inline long wali_syscall_close(int fd) { return syscall(SYS_close, fd); }
static inline long wali_syscall_open(const char *pathname, int flags, int mode) {
#ifdef SYS_open
    return syscall(SYS_open, pathname, flags, mode);
#else
    return syscall(SYS_openat, AT_FDCWD, pathname, flags, mode);
#endif
}
static inline long wali_syscall_fchownat(int dirfd, const char *pathname, int owner, int group, int flags) {
    return syscall(SYS_fchownat, dirfd, pathname, owner, group, flags);
}
static inline long wali_syscall_chown(const char *pathname, int owner, int group) {
#ifdef SYS_chown
    return syscall(SYS_chown, pathname, owner, group);
#else
    return syscall(SYS_fchownat, AT_FDCWD, pathname, owner, group, 0);
#endif
}
static inline long wali_syscall_chmod(const char *pathname, int mode) {
#ifdef SYS_chmod
    return syscall(SYS_chmod, pathname, mode);
#else
    return syscall(SYS_fchmodat, AT_FDCWD, pathname, mode, 0);
#endif
}
static inline long wali_syscall_fchmod(int fd, int mode) { return syscall(SYS_fchmod, fd, mode); }
static inline long wali_syscall_stat(const char *pathname, struct stat *statbuf) {
#ifdef SYS_stat
    return syscall(SYS_stat, pathname, statbuf);
#else
    return syscall(SYS_newfstatat, AT_FDCWD, pathname, statbuf, 0);
#endif
}
static inline long wali_syscall_pipe2(int pipefd[2], int flags) { return syscall(SYS_pipe2, pipefd, flags); }
static inline long wali_syscall_dup(int fd) { return syscall(SYS_dup, fd); }

// Network
static inline long wali_syscall_socket(int domain, int type, int protocol) { return syscall(SYS_socket, domain, type, protocol); }
static inline long wali_syscall_bind(int sockfd, const void *addr, int addrlen) { return syscall(SYS_bind, sockfd, addr, addrlen); }
static inline long wali_syscall_listen(int sockfd, int backlog) { return syscall(SYS_listen, sockfd, backlog); }
static inline long wali_syscall_accept(int sockfd, void *addr, void *addrlen) {
#ifdef SYS_accept
    return syscall(SYS_accept, sockfd, addr, addrlen);
#else
    return syscall(SYS_accept4, sockfd, addr, addrlen, 0);
#endif
}
static inline long wali_syscall_accept4(int sockfd, void *addr, void *addrlen, int flags) { 
#ifdef SYS_accept4
    return syscall(SYS_accept4, sockfd, addr, addrlen, flags);
#else
    // Fallback if needed, but for now strict
    return -1; 
#endif
}
static inline long wali_syscall_connect(int sockfd, const void *addr, int addrlen) { return syscall(SYS_connect, sockfd, addr, addrlen); }
static inline long wali_syscall_getsockname(int sockfd, void *addr, void *addrlen) { return syscall(SYS_getsockname, sockfd, addr, addrlen); }

// Process
static inline long wali_syscall_fork(void) {
#ifdef SYS_fork
    return syscall(SYS_fork);
#else
    return syscall(SYS_clone, SIGCHLD, 0, 0, 0, 0);
#endif
}
static inline void wali_syscall_exit(int status) { syscall(SYS_exit, status); }
static inline long wali_syscall_wait4(int pid, int *status, int options, void *rusage) { return syscall(SYS_wait4, pid, status, options, rusage); }
// static inline long wali_syscall_brk(void *addr) { return syscall(SYS_brk, addr); }

// Misc
static inline long wali_syscall_fcntl(int fd, int cmd, long arg) { return syscall(SYS_fcntl, fd, cmd, arg); }
static inline long wali_syscall_access(const char *pathname, int mode) {
#ifdef SYS_access
    return syscall(SYS_access, pathname, mode);
#else
    return syscall(SYS_faccessat, AT_FDCWD, pathname, mode, 0);
#endif
}
// static inline long wali_syscall_alarm(unsigned int seconds) { return syscall(SYS_alarm, seconds); }
static inline long wali_syscall_rt_sigaction(int signum, const void *act, void *oldact, size_t sigsetsize) { return syscall(SYS_rt_sigaction, signum, act, oldact, sigsetsize); }

// Time
static inline long wali_syscall_clock_gettime(clockid_t clock_id, struct timespec *tp) { return syscall(SYS_clock_gettime, clock_id, tp); }
static inline long wali_syscall_clock_getres(clockid_t clock_id, struct timespec *res) { return syscall(SYS_clock_getres, clock_id, res); }
static inline long wali_syscall_clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain) { return syscall(SYS_clock_nanosleep, clock_id, flags, request, remain); }

#endif

#endif // WALI_SYSCALL_UTILS_H
