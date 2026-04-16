/*
 * struct_layouts.c - Print sizeof/offsetof for all WALI ABI structs.
 * Compare output against: python scripts/types_abi.py
 *
 * NOTE: Structs with pointer or size_t fields will differ between
 * native x86_64 (8-byte ptrs) and wasm32 (4-byte ptrs).
 */
#include <stddef.h>
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <dirent.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <poll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/vfs.h>
#include <sys/sysinfo.h>
#include <utime.h>
#include <sys/utsname.h>
#include <sys/select.h>
#include <sched.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <linux/stat.h>

#define STRUCT_HDR(stype) \
    printf("\n%s (size=%zu):\n", #stype, sizeof(stype))

#define FIELD(stype, fname) \
    printf("  +%4zu  %-24s (%zu bytes)\n", \
        offsetof(stype, fname), #fname, sizeof(((stype*)0)->fname))

#define STRUCT_END(stype) \
    printf("  total: %zu bytes\n", sizeof(stype))

int main() {
    /* struct timespec */
    STRUCT_HDR(struct timespec);
    FIELD(struct timespec, tv_sec);
    FIELD(struct timespec, tv_nsec);
    STRUCT_END(struct timespec);

    /* struct timeval */
    STRUCT_HDR(struct timeval);
    FIELD(struct timeval, tv_sec);
    FIELD(struct timeval, tv_usec);
    STRUCT_END(struct timeval);

    /* struct timezone */
    STRUCT_HDR(struct timezone);
    FIELD(struct timezone, tz_minuteswest);
    FIELD(struct timezone, tz_dsttime);
    STRUCT_END(struct timezone);

    /* stack_t (has pointer — differs on wasm32) */
    STRUCT_HDR(stack_t);
    FIELD(stack_t, ss_sp);
    FIELD(stack_t, ss_flags);
    FIELD(stack_t, ss_size);
    STRUCT_END(stack_t);

    /* struct dirent */
    STRUCT_HDR(struct dirent);
    FIELD(struct dirent, d_ino);
    FIELD(struct dirent, d_off);
    FIELD(struct dirent, d_reclen);
    FIELD(struct dirent, d_type);
    FIELD(struct dirent, d_name);
    STRUCT_END(struct dirent);

    /* struct epoll_event */
    STRUCT_HDR(struct epoll_event);
    FIELD(struct epoll_event, events);
    FIELD(struct epoll_event, data);
    STRUCT_END(struct epoll_event);

    /* struct iovec (has pointer — differs on wasm32) */
    STRUCT_HDR(struct iovec);
    FIELD(struct iovec, iov_base);
    FIELD(struct iovec, iov_len);
    STRUCT_END(struct iovec);

    /* struct pollfd */
    STRUCT_HDR(struct pollfd);
    FIELD(struct pollfd, fd);
    FIELD(struct pollfd, events);
    FIELD(struct pollfd, revents);
    STRUCT_END(struct pollfd);

    /* struct rlimit */
    STRUCT_HDR(struct rlimit);
    FIELD(struct rlimit, rlim_cur);
    FIELD(struct rlimit, rlim_max);
    STRUCT_END(struct rlimit);

    /* struct sigaction (field names are libc-specific) */
    STRUCT_HDR(struct sigaction);
    FIELD(struct sigaction, sa_handler);
    FIELD(struct sigaction, sa_mask);
    FIELD(struct sigaction, sa_flags);
    FIELD(struct sigaction, sa_restorer);
    STRUCT_END(struct sigaction);

    /* struct sockaddr */
    STRUCT_HDR(struct sockaddr);
    FIELD(struct sockaddr, sa_family);
    FIELD(struct sockaddr, sa_data);
    STRUCT_END(struct sockaddr);

    /* struct statfs */
    STRUCT_HDR(struct statfs);
    FIELD(struct statfs, f_type);
    FIELD(struct statfs, f_bsize);
    FIELD(struct statfs, f_blocks);
    FIELD(struct statfs, f_bfree);
    FIELD(struct statfs, f_bavail);
    FIELD(struct statfs, f_files);
    FIELD(struct statfs, f_ffree);
    FIELD(struct statfs, f_fsid);
    FIELD(struct statfs, f_namelen);
    FIELD(struct statfs, f_frsize);
    FIELD(struct statfs, f_flags);
    FIELD(struct statfs, f_spare);
    STRUCT_END(struct statfs);

    ///* struct statx_timestamp (stx_time in types_abi) */
    //STRUCT_HDR(struct statx_timestamp);
    //FIELD(struct statx_timestamp, tv_sec);
    //FIELD(struct statx_timestamp, tv_nsec);
    ///* pad: unnamed/__reserved */
    //STRUCT_END(struct statx_timestamp);

    /* struct sysinfo */
    STRUCT_HDR(struct sysinfo);
    FIELD(struct sysinfo, uptime);
    FIELD(struct sysinfo, loads);
    FIELD(struct sysinfo, totalram);
    FIELD(struct sysinfo, freeram);
    FIELD(struct sysinfo, sharedram);
    FIELD(struct sysinfo, bufferram);
    FIELD(struct sysinfo, totalswap);
    FIELD(struct sysinfo, freeswap);
    FIELD(struct sysinfo, procs);
    /* pad: unnamed */
    FIELD(struct sysinfo, totalhigh);
    FIELD(struct sysinfo, freehigh);
    FIELD(struct sysinfo, mem_unit);
    STRUCT_END(struct sysinfo);

    /* struct utimbuf */
    STRUCT_HDR(struct utimbuf);
    FIELD(struct utimbuf, actime);
    FIELD(struct utimbuf, modtime);
    STRUCT_END(struct utimbuf);

    /* struct utsname */
    STRUCT_HDR(struct utsname);
    FIELD(struct utsname, sysname);
    FIELD(struct utsname, nodename);
    FIELD(struct utsname, release);
    FIELD(struct utsname, version);
    FIELD(struct utsname, machine);
    FIELD(struct utsname, domainname);
    STRUCT_END(struct utsname);

    /* fd_set (opaque — sizeof only) */
    printf("\nfd_set (size=%zu):\n", sizeof(fd_set));
    printf("  total: %zu bytes\n", sizeof(fd_set));

    /* cpu_set_t (opaque — may not exist on all platforms) */
    printf("\ncpu_set_t (size=%zu):\n", sizeof(cpu_set_t));
    printf("  total: %zu bytes\n", sizeof(cpu_set_t));

    /* struct itimerval */
    STRUCT_HDR(struct itimerval);
    FIELD(struct itimerval, it_interval);
    FIELD(struct itimerval, it_value);
    STRUCT_END(struct itimerval);

    /* struct msghdr (has pointers — differs on wasm32) */
    STRUCT_HDR(struct msghdr);
    FIELD(struct msghdr, msg_name);
    FIELD(struct msghdr, msg_namelen);
    FIELD(struct msghdr, msg_iov);
    FIELD(struct msghdr, msg_iovlen);
    FIELD(struct msghdr, msg_control);
    FIELD(struct msghdr, msg_controllen);
    FIELD(struct msghdr, msg_flags);
    STRUCT_END(struct msghdr);

    /* struct rusage */
    STRUCT_HDR(struct rusage);
    FIELD(struct rusage, ru_utime);
    FIELD(struct rusage, ru_stime);
    FIELD(struct rusage, ru_maxrss);
    FIELD(struct rusage, ru_ixrss);
    FIELD(struct rusage, ru_idrss);
    FIELD(struct rusage, ru_isrss);
    FIELD(struct rusage, ru_minflt);
    FIELD(struct rusage, ru_majflt);
    FIELD(struct rusage, ru_nswap);
    FIELD(struct rusage, ru_inblock);
    FIELD(struct rusage, ru_oublock);
    FIELD(struct rusage, ru_msgsnd);
    FIELD(struct rusage, ru_msgrcv);
    FIELD(struct rusage, ru_nsignals);
    FIELD(struct rusage, ru_nvcsw);
    FIELD(struct rusage, ru_nivcsw);
    STRUCT_END(struct rusage);

    /* struct stat */
    STRUCT_HDR(struct stat);
    FIELD(struct stat, st_dev);
    FIELD(struct stat, st_ino);
    FIELD(struct stat, st_nlink);
    FIELD(struct stat, st_mode);
    FIELD(struct stat, st_uid);
    FIELD(struct stat, st_gid);
    /* pad0: __pad0 */
    FIELD(struct stat, st_rdev);
    FIELD(struct stat, st_size);
    FIELD(struct stat, st_blksize);
    FIELD(struct stat, st_blocks);
    FIELD(struct stat, st_atim);
    FIELD(struct stat, st_mtim);
    FIELD(struct stat, st_ctim);
    STRUCT_END(struct stat);

    /* Runtime validation via fstatat — verifies struct stat layout works
     * through the WALI ABI boundary. If the layout is correct, the fields
     * will contain sensible values for "." */
    printf("\n--- fstatat runtime check ---\n");
    struct stat rt;
    int rc = fstatat(AT_FDCWD, ".", &rt, 0);
    if (rc < 0) {
        printf("fstatat failed: %d\n", rc);
    } else {
        printf("fstatat OK: st_mode=0%o st_size=%lld st_nlink=%lu\n",
               (unsigned)rt.st_mode, (long long)rt.st_size, (unsigned long)rt.st_nlink);
    }

    return 0;
}
