## Syscall Support

The WALI prototype is designed to support syscalls across the following
architectures: **x86-64, aarch64, riscv64**

Generated based on Marcin Juszkiewicz's [syscall table](https://gpages.juszkiewicz.com.pl/syscalls-table/syscalls.html)

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

* personality
* execveat
* riscv\_flush\_icache
* mq\_unlink
* waitid
* timer\_create
* shmctl
* process\_madvise
* epoll\_wait\_old
* quotactl
* msgget
* mount\_setattr
* signalfd4
* kexec\_load
* nfsservctl
* fsopen
* set\_thread\_area
* fanotify\_mark
* umount2
* sched\_getaffinity
* setregid
* inotify\_rm\_watch
* lchown
* landlock\_create\_ruleset
* fsmount
* acct
* listxattr
* setns
* swapon
* fanotify\_init
* ioperm
* timerfd\_settime
* process\_mrelease
* bpf
* fsconfig
* removexattr
* sched\_rr\_get\_interval
* prctl
* pause
* getpriority
* keyctl
* mlock
* io\_uring\_register
* mq\_timedsend
* sethostname
* llistxattr
* mq\_getsetattr
* io\_setup
* io\_pgetevents
* vmsplice
* pidfd\_open
* set\_mempolicy\_home\_node
* io\_uring\_enter
* set\_robust\_list
* add\_key
* mq\_open
* unshare
* get\_kernel\_syms
* fadvise64
* pivot\_root
* perf\_event\_open
* msgsnd
* utimes
* rt\_sigtimedwait
* epoll\_wait
* setfsuid
* msgctl
* munlockall
* lsetxattr
* syncfs
* io\_getevents
* io\_uring\_setup
* sendmmsg
* signalfd
* tgkill
* kcmp
* pkey\_mprotect
* clock\_adjtime
* shmat
* process\_vm\_readv
* ptrace
* clock\_settime
* sched\_getparam
* getitimer
* setdomainname
* shmdt
* reboot
* open\_tree
* times
* mknodat
* get\_mempolicy
* mlock2
* openat2
* capget
* set\_mempolicy
* iopl
* timer\_getoverrun
* pidfd\_getfd
* sched\_get\_priority\_min
* map\_shadow\_stack
* get\_robust\_list
* preadv
* fchmodat2
* epoll\_create
* sysfs
* shmget
* futex\_waitv
* msgrcv
* pkey\_alloc
* membarrier
* setxattr
* getxattr
* adjtimex
* timerfd\_gettime
* open\_by\_handle\_at
* mq\_notify
* io\_cancel
* getcpu
* quotactl\_fd
* truncate
* getresgid
* sched\_setaffinity
* readahead
* swapoff
* fgetxattr
* restart\_syscall
* preadv2
* lookup\_dcookie
* inotify\_add\_watch
* gettimeofday
* sched\_setparam
* sendfile
* sched\_getscheduler
* clone3
* \_sysctl
* pwritev2
* mincore
* epoll\_ctl\_old
* settimeofday
* sched\_getattr
* sched\_get\_priority\_max
* syslog
* create\_module
* time
* cachestat
* getpgrp
* io\_destroy
* semtimedop
* rt\_tgsigqueueinfo
* mknod
* landlock\_add\_rule
* inotify\_init
* lremovexattr
* ustat
* flistxattr
* epoll\_pwait2
* modify\_ldt
* semop
* request\_key
* copy\_file\_range
* timerfd\_create
* fsetxattr
* creat
* migrate\_pages
* ioprio\_set
* tee
* fspick
* capset
* userfaultfd
* rseq
* recvmmsg
* getpmsg
* sched\_setattr
* vfork
* setreuid
* seccomp
* close\_range
* setfsgid
* get\_thread\_area
* mount
* semctl
* mbind
* pwritev
* riscv\_hwprobe
* process\_vm\_writev
* splice
* clone
* sync
* finit\_module
* memfd\_secret
* vhangup
* renameat
* mlockall
* semget
* futimesat
* remap\_file\_pages
* inotify\_init1
* ioprio\_get
* timer\_delete
* name\_to\_handle\_at
* timer\_gettime
* getresuid
* query\_module
* rt\_sigqueueinfo
* uselib
* timer\_settime
* pkey\_free
* arch\_prctl
* move\_mount
* kexec\_file\_load
* pidfd\_send\_signal
* mq\_timedreceive
* munlock
* fremovexattr
* fallocate
* memfd\_create
* io\_submit
* sync\_file\_range
* landlock\_restrict\_self
* move\_pages
* sched\_setscheduler
* delete\_module
* lgetxattr
* init\_module
* setpriority
