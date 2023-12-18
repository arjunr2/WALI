## Syscall Support

The WALI prototype is designed to support syscalls across the following
architectures: **x86-64, aarch64, riscv64**

Generated based on Marcin Juszkiewicz's [syscall table](https://gpages.juszkiewicz.com.pl/syscalls-table/syscalls.html)

Number of supported syscalls: **138**

| Syscall         |   # Args | a1               | a2                  | a3                | a4                  | a5               | a6         |
|:----------------|---------:|:-----------------|:--------------------|:------------------|:--------------------|:-----------------|:-----------|
| read            |        3 | int              | void\*               | size\_t            |                     |                  |            |
| write           |        3 | int              | void\*               | size\_t            |                     |                  |            |
| open            |        3 | char\*            | int                 | mode\_t            |                     |                  |            |
| close           |        1 | int              |                     |                   |                     |                  |            |
| stat            |        2 | char\*            | struct stat\*        |                   |                     |                  |            |
| fstat           |        2 | int              | struct stat\*        |                   |                     |                  |            |
| lstat           |        2 | char\*            | struct stat\*        |                   |                     |                  |            |
| poll            |        3 | struct pollfd\*   | nfds\_t              | int               |                     |                  |            |
| lseek           |        3 | int              | off\_t               | int               |                     |                  |            |
| mmap            |        6 | void\*            | size\_t              | int               | int                 | int              | off\_t      |
| mprotect        |        3 | void\*            | size\_t              | int               |                     |                  |            |
| munmap          |        2 | void\*            | size\_t              |                   |                     |                  |            |
| brk             |        1 | void\*            |                     |                   |                     |                  |            |
| rt\_sigaction    |        4 | int              | struct sigaction\*   | struct sigaction\* | size\_t              |                  |            |
| rt\_sigprocmask  |        4 | int              | sigset\_t\*           | sigset\_t\*         | size\_t              |                  |            |
| rt\_sigreturn    |        1 | long             |                     |                   |                     |                  |            |
| ioctl           |        3 | int              | int                 | char\*             |                     |                  |            |
| pread64         |        4 | int              | char\*               | size\_t            | off\_t               |                  |            |
| pwrite64        |        4 | int              | char\*               | size\_t            | off\_t               |                  |            |
| readv           |        3 | int              | struct iovec\*       | int               |                     |                  |            |
| writev          |        3 | int              | struct iovec\*       | int               |                     |                  |            |
| access          |        2 | char\*            | int                 |                   |                     |                  |            |
| pipe            |        1 | int\*             |                     |                   |                     |                  |            |
| select          |        5 | int              | fd\_set\*             | fd\_set\*           | fd\_set\*             | struct timeval\*  |            |
| sched\_yield     |        0 |                  |                     |                   |                     |                  |            |
| mremap          |        5 | void\*            | size\_t              | size\_t            | int                 | void\*            |            |
| msync           |        3 | void\*            | size\_t              | int               |                     |                  |            |
| madvise         |        3 | void\*            | size\_t              | int               |                     |                  |            |
| dup             |        1 | int              |                     |                   |                     |                  |            |
| dup2            |        2 | int              | int                 |                   |                     |                  |            |
| nanosleep       |        2 | struct timespec\* | struct timespec\*    |                   |                     |                  |            |
| alarm           |        1 | int              |                     |                   |                     |                  |            |
| setitimer       |        3 | int              | struct itimerval\*   | struct itimerval\* |                     |                  |            |
| getpid          |        0 |                  |                     |                   |                     |                  |            |
| socket          |        3 | int              | int                 | int               |                     |                  |            |
| connect         |        3 | int              | struct sockaddr\*    | socklen\_t         |                     |                  |            |
| accept          |        3 | int              | struct sockaddr\*    | socklen\_t\*        |                     |                  |            |
| sendto          |        6 | int              | void\*               | size\_t            | int                 | struct sockaddr\* | socklen\_t  |
| recvfrom        |        6 | int              | void\*               | size\_t            | int                 | struct sockaddr\* | socklen\_t\* |
| sendmsg         |        3 | int              | struct msghdr\*      | int               |                     |                  |            |
| recvmsg         |        3 | int              | struct msghdr\*      | int               |                     |                  |            |
| shutdown        |        2 | int              | int                 |                   |                     |                  |            |
| bind            |        3 | int              | struct sockaddr\*    | socklen\_t         |                     |                  |            |
| listen          |        2 | int              | int                 |                   |                     |                  |            |
| getsockname     |        3 | int              | struct sockaddr\*    | socklen\_t\*        |                     |                  |            |
| getpeername     |        3 | int              | struct sockaddr\*    | socklen\_t\*        |                     |                  |            |
| socketpair      |        4 | int              | int                 | int               | int\*                |                  |            |
| setsockopt      |        5 | int              | int                 | int               | void\*               | socklen\_t        |            |
| getsockopt      |        5 | int              | int                 | int               | void\*               | socklen\_t\*       |            |
| fork            |        0 |                  |                     |                   |                     |                  |            |
| execve          |        3 | char\*            | char\*               | char\*             |                     |                  |            |
| exit            |        1 | int              |                     |                   |                     |                  |            |
| wait4           |        4 | pid\_t            | int\*                | int               | struct rusage\*      |                  |            |
| kill            |        2 | pid\_t            | int                 |                   |                     |                  |            |
| uname           |        1 | struct utsname\*  |                     |                   |                     |                  |            |
| fcntl           |        3 | int              | int                 | int               |                     |                  |            |
| flock           |        2 | int              | int                 |                   |                     |                  |            |
| fsync           |        1 | int              |                     |                   |                     |                  |            |
| fdatasync       |        1 | int              |                     |                   |                     |                  |            |
| ftruncate       |        2 | int              | off\_t               |                   |                     |                  |            |
| getdents        |        3 | int              | struct dirent\*      | int               |                     |                  |            |
| getcwd          |        2 | char\*            | size\_t              |                   |                     |                  |            |
| chdir           |        1 | char\*            |                     |                   |                     |                  |            |
| fchdir          |        1 | int              |                     |                   |                     |                  |            |
| rename          |        2 | char\*            | char\*               |                   |                     |                  |            |
| mkdir           |        2 | char\*            | mode\_t              |                   |                     |                  |            |
| rmdir           |        1 | char\*            |                     |                   |                     |                  |            |
| link            |        2 | char\*            | char\*               |                   |                     |                  |            |
| unlink          |        1 | char\*            |                     |                   |                     |                  |            |
| symlink         |        2 | char\*            | char\*               |                   |                     |                  |            |
| readlink        |        3 | char\*            | char\*               | size\_t            |                     |                  |            |
| chmod           |        2 | char\*            | mode\_t              |                   |                     |                  |            |
| fchmod          |        2 | int              | mode\_t              |                   |                     |                  |            |
| chown           |        3 | char\*            | uid\_t               | gid\_t             |                     |                  |            |
| fchown          |        3 | int              | uid\_t               | gid\_t             |                     |                  |            |
| umask           |        1 | mode\_t           |                     |                   |                     |                  |            |
| getrlimit       |        2 | int              | struct rlimit\*      |                   |                     |                  |            |
| getrusage       |        2 | int              | struct rusage\*      |                   |                     |                  |            |
| sysinfo         |        1 | struct sysinfo\*  |                     |                   |                     |                  |            |
| getuid          |        0 |                  |                     |                   |                     |                  |            |
| getgid          |        0 |                  |                     |                   |                     |                  |            |
| setuid          |        1 | uid\_t            |                     |                   |                     |                  |            |
| setgid          |        1 | gid\_t            |                     |                   |                     |                  |            |
| geteuid         |        0 |                  |                     |                   |                     |                  |            |
| getegid         |        0 |                  |                     |                   |                     |                  |            |
| setpgid         |        2 | pid\_t            | pid\_t               |                   |                     |                  |            |
| getppid         |        0 |                  |                     |                   |                     |                  |            |
| setsid          |        0 |                  |                     |                   |                     |                  |            |
| getgroups       |        2 | size\_t           | gid\_t\*              |                   |                     |                  |            |
| setgroups       |        2 | size\_t           | gid\_t\*              |                   |                     |                  |            |
| setresuid       |        3 | uid\_t            | uid\_t               | uid\_t             |                     |                  |            |
| setresgid       |        3 | gid\_t            | gid\_t               | gid\_t             |                     |                  |            |
| getpgid         |        1 | pid\_t            |                     |                   |                     |                  |            |
| getsid          |        1 | pid\_t            |                     |                   |                     |                  |            |
| rt\_sigpending   |        2 | sigset\_t\*        | size\_t              |                   |                     |                  |            |
| rt\_sigsuspend   |        2 | sigset\_t\*        | size\_t              |                   |                     |                  |            |
| sigaltstack     |        2 | stack\_t\*         | stack\_t\*            |                   |                     |                  |            |
| utime           |        2 | char\*            | struct utimbuf\*     |                   |                     |                  |            |
| statfs          |        2 | char\*            | struct statfs\*      |                   |                     |                  |            |
| fstatfs         |        2 | int              | struct statfs\*      |                   |                     |                  |            |
| setrlimit       |        2 | int              | struct rlimit\*      |                   |                     |                  |            |
| chroot          |        1 | char\*            |                     |                   |                     |                  |            |
| gettid          |        0 |                  |                     |                   |                     |                  |            |
| tkill           |        2 | int              | int                 |                   |                     |                  |            |
| futex           |        6 | int\*             | int                 | int               | struct timespec\*    | int\*             | int        |
| getdents64      |        3 | int              | struct dirent\*      | int               |                     |                  |            |
| set\_tid\_address |        1 | int\*             |                     |                   |                     |                  |            |
| fadvise         |        4 | int              | off\_t               | off\_t             | int                 |                  |            |
| clock\_gettime   |        2 | clockid\_t        | struct timespec\*    |                   |                     |                  |            |
| clock\_getres    |        2 | clockid\_t        | struct timespec\*    |                   |                     |                  |            |
| clock\_nanosleep |        4 | clockid\_t        | int                 | struct timespec\*  | struct timespec\*    |                  |            |
| exit\_group      |        1 | int              |                     |                   |                     |                  |            |
| epoll\_ctl       |        4 | int              | int                 | int               | struct epoll\_event\* |                  |            |
| openat          |        4 | int              | char\*               | int               | mode\_t              |                  |            |
| mkdirat         |        3 | int              | char\*               | mode\_t            |                     |                  |            |
| fchownat        |        5 | int              | char\*               | uid\_t             | gid\_t               | int              |            |
| newfstatat      |        4 | int              | char\*               | struct stat\*      | int                 |                  |            |
| unlinkat        |        3 | int              | char\*               | int               |                     |                  |            |
| linkat          |        5 | int              | char\*               | int               | char\*               | int              |            |
| symlinkat       |        3 | char\*            | int                 | char\*             |                     |                  |            |
| readlinkat      |        4 | int              | char\*               | char\*             | size\_t              |                  |            |
| fchmodat        |        4 | int              | char\*               | mode\_t            | int                 |                  |            |
| faccessat       |        4 | int              | char\*               | int               | int                 |                  |            |
| pselect6        |        6 | int              | fd\_set\*             | fd\_set\*           | fd\_set\*             | struct timespec\* | void\*      |
| ppoll           |        5 | struct pollfd\*   | nfds\_t              | struct timespec\*  | sigset\_t\*           | size\_t           |            |
| utimensat       |        4 | int              | char\*               | struct timespec\*  | int                 |                  |            |
| epoll\_pwait     |        6 | int              | struct epoll\_event\* | int               | int                 | sigset\_t\*        | size\_t     |
| eventfd         |        1 | int              |                     |                   |                     |                  |            |
| accept4         |        4 | int              | struct sockaddr\*    | socklen\_t\*        | int                 |                  |            |
| eventfd2        |        2 | int              | int                 |                   |                     |                  |            |
| epoll\_create1   |        1 | int              |                     |                   |                     |                  |            |
| dup3            |        3 | int              | int                 | int               |                     |                  |            |
| pipe2           |        2 | int\*             | int                 |                   |                     |                  |            |
| prlimit64       |        4 | int              | int                 | struct rlimit\*    | struct rlimit\*      |                  |            |
| renameat2       |        5 | int              | char\*               | int               | char\*               | int              |            |
| getrandom       |        3 | void\*            | size\_t              | int               |                     |                  |            |
| statx           |        5 | int              | char\*               | int               | int                 | struct statx\*    |            |
| faccessat2      |        4 | int              | char\*               | int               | int                 |                  |            |

## Currently Unsupported Syscalls

* \_sysctl
* acct
* add\_key
* adjtimex
* arch\_prctl
* bpf
* cachestat
* capget
* capset
* clock\_adjtime
* clock\_settime
* clone
* clone3
* close\_range
* copy\_file\_range
* creat
* create\_module
* delete\_module
* epoll\_create
* epoll\_ctl\_old
* epoll\_pwait2
* epoll\_wait
* epoll\_wait\_old
* execveat
* fadvise64
* fallocate
* fanotify\_init
* fanotify\_mark
* fchmodat2
* fgetxattr
* finit\_module
* flistxattr
* fremovexattr
* fsconfig
* fsetxattr
* fsmount
* fsopen
* fspick
* futex\_waitv
* futimesat
* get\_kernel\_syms
* get\_mempolicy
* get\_robust\_list
* get\_thread\_area
* getcpu
* getitimer
* getpgrp
* getpmsg
* getpriority
* getresgid
* getresuid
* gettimeofday
* getxattr
* init\_module
* inotify\_add\_watch
* inotify\_init
* inotify\_init1
* inotify\_rm\_watch
* io\_cancel
* io\_destroy
* io\_getevents
* io\_pgetevents
* io\_setup
* io\_submit
* io\_uring\_enter
* io\_uring\_register
* io\_uring\_setup
* ioperm
* iopl
* ioprio\_get
* ioprio\_set
* kcmp
* kexec\_file\_load
* kexec\_load
* keyctl
* landlock\_add\_rule
* landlock\_create\_ruleset
* landlock\_restrict\_self
* lchown
* lgetxattr
* listxattr
* llistxattr
* lookup\_dcookie
* lremovexattr
* lsetxattr
* map\_shadow\_stack
* mbind
* membarrier
* memfd\_create
* memfd\_secret
* migrate\_pages
* mincore
* mknod
* mknodat
* mlock
* mlock2
* mlockall
* modify\_ldt
* mount
* mount\_setattr
* move\_mount
* move\_pages
* mq\_getsetattr
* mq\_notify
* mq\_open
* mq\_timedreceive
* mq\_timedsend
* mq\_unlink
* msgctl
* msgget
* msgrcv
* msgsnd
* munlock
* munlockall
* name\_to\_handle\_at
* nfsservctl
* open\_by\_handle\_at
* open\_tree
* openat2
* pause
* perf\_event\_open
* personality
* pidfd\_getfd
* pidfd\_open
* pidfd\_send\_signal
* pivot\_root
* pkey\_alloc
* pkey\_free
* pkey\_mprotect
* prctl
* preadv
* preadv2
* process\_madvise
* process\_mrelease
* process\_vm\_readv
* process\_vm\_writev
* ptrace
* pwritev
* pwritev2
* query\_module
* quotactl
* quotactl\_fd
* readahead
* reboot
* recvmmsg
* remap\_file\_pages
* removexattr
* renameat
* request\_key
* restart\_syscall
* riscv\_flush\_icache
* riscv\_hwprobe
* rseq
* rt\_sigqueueinfo
* rt\_sigtimedwait
* rt\_tgsigqueueinfo
* sched\_get\_priority\_max
* sched\_get\_priority\_min
* sched\_getaffinity
* sched\_getattr
* sched\_getparam
* sched\_getscheduler
* sched\_rr\_get\_interval
* sched\_setaffinity
* sched\_setattr
* sched\_setparam
* sched\_setscheduler
* seccomp
* semctl
* semget
* semop
* semtimedop
* sendfile
* sendmmsg
* set\_mempolicy
* set\_mempolicy\_home\_node
* set\_robust\_list
* set\_thread\_area
* setdomainname
* setfsgid
* setfsuid
* sethostname
* setns
* setpriority
* setregid
* setreuid
* settimeofday
* setxattr
* shmat
* shmctl
* shmdt
* shmget
* signalfd
* signalfd4
* splice
* swapoff
* swapon
* sync
* sync\_file\_range
* syncfs
* sysfs
* syslog
* tee
* tgkill
* time
* timer\_create
* timer\_delete
* timer\_getoverrun
* timer\_gettime
* timer\_settime
* timerfd\_create
* timerfd\_gettime
* timerfd\_settime
* times
* truncate
* umount2
* unshare
* uselib
* userfaultfd
* ustat
* utimes
* vfork
* vhangup
* vmsplice
* waitid
