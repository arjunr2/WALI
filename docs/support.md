## Syscall Support

The WALI prototype is designed to support syscalls across the following
architectures: **x86-64, aarch64, riscv64**

Uses metadata from Marcin Juszkiewicz's [syscall table](https://gpages.juszkiewicz.com.pl/syscalls-table/syscalls.html)

Number of supported syscalls: **141**

| Syscall | # Args | a1 | a2 | a3 | a4 | a5 | a6 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| read | 3 | int fd | void\* buf | size\_t count |  |  |  |
| write | 3 | int fd | void\* buf | size\_t count |  |  |  |
| open | 3 | char\* pathname | int flags | mode\_t mode |  |  |  |
| close | 1 | int fd |  |  |  |  |  |
| stat | 2 | char\* pathname | struct stat\* statbuf |  |  |  |  |
| fstat | 2 | int fd | struct stat\* statbuf |  |  |  |  |
| lstat | 2 | char\* pathname | struct stat\* statbuf |  |  |  |  |
| poll | 3 | struct pollfd\* fds | nfds\_t nfds | int timeout |  |  |  |
| lseek | 3 | int fd | off\_t offset | int whence |  |  |  |
| mmap | 6 | void\* addr | size\_t length | int prot | int flags | int fd | off\_t offset |
| mprotect | 3 | void\* addr | size\_t len | int prot |  |  |  |
| munmap | 2 | void\* addr | size\_t len |  |  |  |  |
| brk | 1 | void\* addr |  |  |  |  |  |
| rt\_sigaction | 4 | int signum | struct sigaction\* act | struct sigaction\* oldact | size\_t sigsetsize |  |  |
| rt\_sigprocmask | 4 | int how | sigset\_t\* set | sigset\_t\* oldset | size\_t sigsetsize |  |  |
| rt\_sigreturn | 1 | long unused |  |  |  |  |  |
| ioctl | 3 | int fd | int request | char\* argp |  |  |  |
| pread64 | 4 | int fd | char\* buf | size\_t count | off\_t offset |  |  |
| pwrite64 | 4 | int fd | char\* buf | size\_t count | off\_t offset |  |  |
| readv | 3 | int fd | struct iovec\* iov | int iovcnt |  |  |  |
| writev | 3 | int fd | struct iovec\* iov | int iovcnt |  |  |  |
| access | 2 | char\* pathname | int mode |  |  |  |  |
| pipe | 1 | int\* pipefd |  |  |  |  |  |
| select | 5 | int nfds | fd\_set\* readfds | fd\_set\* writefds | fd\_set\* exceptfds | struct timeval\* timeout |  |
| sched\_yield | 0 |  |  |  |  |  |  |
| mremap | 5 | void\* old\_address | size\_t old\_size | size\_t new\_size | int flags | void\* new\_address |  |
| msync | 3 | void\* addr | size\_t length | int flags |  |  |  |
| madvise | 3 | void\* addr | size\_t length | int advice |  |  |  |
| dup | 1 | int oldfd |  |  |  |  |  |
| dup2 | 2 | int oldfd | int newfd |  |  |  |  |
| nanosleep | 2 | struct timespec\* req | struct timespec\* rem |  |  |  |  |
| alarm | 1 | int seconds |  |  |  |  |  |
| setitimer | 3 | int which | struct itimerval\* new\_value | struct itimerval\* old\_value |  |  |  |
| getpid | 0 |  |  |  |  |  |  |
| socket | 3 | int domain | int type | int protocol |  |  |  |
| connect | 3 | int sockfd | struct sockaddr\* addr | socklen\_t addrlen |  |  |  |
| accept | 3 | int sockfd | struct sockaddr\* addr | socklen\_t\* addrlen |  |  |  |
| sendto | 6 | int sockfd | void\* buf | size\_t len | int flags | struct sockaddr\* dest\_addr | socklen\_t addrlen |
| recvfrom | 6 | int sockfd | void\* buf | size\_t len | int flags | struct sockaddr\* src\_addr | socklen\_t\* addrlen |
| sendmsg | 3 | int sockfd | struct msghdr\* msg | int flags |  |  |  |
| recvmsg | 3 | int sockfd | struct msghdr\* msg | int flags |  |  |  |
| shutdown | 2 | int sockfd | int how |  |  |  |  |
| bind | 3 | int sockfd | struct sockaddr\* addr | socklen\_t addrlen |  |  |  |
| listen | 2 | int sockfd | int backlog |  |  |  |  |
| getsockname | 3 | int sockfd | struct sockaddr\* addr | socklen\_t\* addrlen |  |  |  |
| getpeername | 3 | int sockfd | struct sockaddr\* addr | socklen\_t\* addrlen |  |  |  |
| socketpair | 4 | int domain | int type | int protocol | int\* sv |  |  |
| setsockopt | 5 | int sockfd | int level | int optname | void\* optval | socklen\_t optlen |  |
| getsockopt | 5 | int sockfd | int level | int optname | void\* optval | socklen\_t\* optlen |  |
| fork | 0 |  |  |  |  |  |  |
| execve | 3 | char\* pathname | char\*\* argv | char\*\* envp |  |  |  |
| exit | 1 | int status |  |  |  |  |  |
| wait4 | 4 | pid\_t pid | int\* wstatus | int options | struct rusage\* rusage |  |  |
| kill | 2 | pid\_t pid | int sig |  |  |  |  |
| uname | 1 | struct utsname\* buf |  |  |  |  |  |
| fcntl | 3 | int fd | int cmd | unsigned long arg |  |  |  |
| flock | 2 | int fd | int operation |  |  |  |  |
| fsync | 1 | int fd |  |  |  |  |  |
| fdatasync | 1 | int fd |  |  |  |  |  |
| ftruncate | 2 | int fd | off\_t length |  |  |  |  |
| getcwd | 2 | char\* buf | size\_t size |  |  |  |  |
| chdir | 1 | char\* path |  |  |  |  |  |
| fchdir | 1 | int fd |  |  |  |  |  |
| rename | 2 | char\* oldpath | char\* newpath |  |  |  |  |
| mkdir | 2 | char\* pathname | mode\_t mode |  |  |  |  |
| rmdir | 1 | char\* pathname |  |  |  |  |  |
| link | 2 | char\* oldpath | char\* newpath |  |  |  |  |
| unlink | 1 | char\* pathname |  |  |  |  |  |
| symlink | 2 | char\* target | char\* linkpath |  |  |  |  |
| readlink | 3 | char\* pathname | char\* buf | size\_t bufsiz |  |  |  |
| chmod | 2 | char\* pathname | mode\_t mode |  |  |  |  |
| fchmod | 2 | int fd | mode\_t mode |  |  |  |  |
| chown | 3 | char\* pathname | uid\_t owner | gid\_t group |  |  |  |
| fchown | 3 | int fd | uid\_t owner | gid\_t group |  |  |  |
| umask | 1 | mode\_t mask |  |  |  |  |  |
| gettimeofday | 2 | struct timeval\* tv | struct timezone\* tz |  |  |  |  |
| getrlimit | 2 | int resource | struct rlimit\* rlim |  |  |  |  |
| getrusage | 2 | int who | struct rusage\* usage |  |  |  |  |
| sysinfo | 1 | struct sysinfo\* info |  |  |  |  |  |
| getuid | 0 |  |  |  |  |  |  |
| getgid | 0 |  |  |  |  |  |  |
| setuid | 1 | uid\_t uid |  |  |  |  |  |
| setgid | 1 | gid\_t gid |  |  |  |  |  |
| geteuid | 0 |  |  |  |  |  |  |
| getegid | 0 |  |  |  |  |  |  |
| setpgid | 2 | pid\_t pid | pid\_t pgid |  |  |  |  |
| getppid | 0 |  |  |  |  |  |  |
| setsid | 0 |  |  |  |  |  |  |
| setreuid | 2 | uid\_t ruid | uid\_t euid |  |  |  |  |
| setregid | 2 | gid\_t rgid | gid\_t egid |  |  |  |  |
| getgroups | 2 | size\_t size | gid\_t\* list |  |  |  |  |
| setgroups | 2 | size\_t size | gid\_t\* list |  |  |  |  |
| setresuid | 3 | uid\_t ruid | uid\_t euid | uid\_t suid |  |  |  |
| setresgid | 3 | gid\_t rgid | gid\_t egid | gid\_t sgid |  |  |  |
| getpgid | 1 | pid\_t pid |  |  |  |  |  |
| getsid | 1 | pid\_t pid |  |  |  |  |  |
| rt\_sigpending | 2 | sigset\_t\* set | size\_t sigsetsize |  |  |  |  |
| rt\_sigsuspend | 2 | sigset\_t\* mask | size\_t sigsetsize |  |  |  |  |
| sigaltstack | 2 | stack\_t\* ss | stack\_t\* old\_ss |  |  |  |  |
| statfs | 2 | char\* path | struct statfs\* buf |  |  |  |  |
| fstatfs | 2 | int fd | struct statfs\* buf |  |  |  |  |
| prctl | 5 | int option | unsigned long arg2 | unsigned long arg3 | unsigned long arg4 | unsigned long arg5 |  |
| setrlimit | 2 | int resource | struct rlimit\* rlim |  |  |  |  |
| chroot | 1 | char\* path |  |  |  |  |  |
| gettid | 0 |  |  |  |  |  |  |
| tkill | 2 | int tid | int sig |  |  |  |  |
| futex | 6 | int\* uaddr | int futex\_op | int val | struct timespec\* timeout | int\* uaddr2 | int val3 |
| sched\_getaffinity | 3 | pid\_t pid | size\_t cpusetsize | cpu\_set\_t\* mask |  |  |  |
| getdents64 | 3 | int fd | struct dirent\* dirp | int count |  |  |  |
| set\_tid\_address | 1 | int\* tidptr |  |  |  |  |  |
| fadvise | 4 | int fd | off\_t offset | off\_t len | int advice |  |  |
| clock\_gettime | 2 | clockid\_t clockid | struct timespec\* tp |  |  |  |  |
| clock\_getres | 2 | clockid\_t clockid | struct timespec\* res |  |  |  |  |
| clock\_nanosleep | 4 | clockid\_t clockid | int flags | struct timespec\* request | struct timespec\* remain |  |  |
| exit\_group | 1 | int status |  |  |  |  |  |
| epoll\_ctl | 4 | int epfd | int op | int fd | struct epoll\_event\* event |  |  |
| openat | 4 | int dirfd | char\* pathname | int flags | mode\_t mode |  |  |
| mkdirat | 3 | int dirfd | char\* pathname | mode\_t mode |  |  |  |
| fchownat | 5 | int dirfd | char\* pathname | uid\_t owner | gid\_t group | int flags |  |
| newfstatat | 4 | int dirfd | char\* pathname | struct stat\* statbuf | int flags |  |  |
| unlinkat | 3 | int dirfd | char\* pathname | int flags |  |  |  |
| linkat | 5 | int olddirfd | char\* oldpath | int newdirfd | char\* newpath | int flags |  |
| symlinkat | 3 | char\* target | int newdirfd | char\* linkpath |  |  |  |
| readlinkat | 4 | int dirfd | char\* pathname | char\* buf | size\_t bufsiz |  |  |
| fchmodat | 4 | int dirfd | char\* pathname | mode\_t mode | int flags |  |  |
| faccessat | 4 | int dirfd | char\* pathname | int mode | int flags |  |  |
| pselect6 | 6 | int nfds | fd\_set\* readfds | fd\_set\* writefds | fd\_set\* exceptfds | struct timespec\* timeout | void\* sigmask |
| ppoll | 5 | struct pollfd\* fds | nfds\_t nfds | struct timespec\* tmo\_p | sigset\_t\* sigmask | size\_t sigsetsize |  |
| utimensat | 4 | int dirfd | char\* pathname | struct timespec\* times | int flags |  |  |
| epoll\_pwait | 6 | int epfd | struct epoll\_event\* events | int maxevents | int timeout | sigset\_t\* sigmask | size\_t sigsetsize |
| eventfd | 1 | int initval |  |  |  |  |  |
| accept4 | 4 | int sockfd | struct sockaddr\* addr | socklen\_t\* addrlen | int flags |  |  |
| eventfd2 | 2 | int initval | int flags |  |  |  |  |
| epoll\_create1 | 1 | int flags |  |  |  |  |  |
| dup3 | 3 | int oldfd | int newfd | int flags |  |  |  |
| pipe2 | 2 | int\* pipefd | int flags |  |  |  |  |
| prlimit64 | 4 | pid\_t pid | int resource | struct rlimit\* new\_limit | struct rlimit\* old\_limit |  |  |
| renameat2 | 5 | int olddirfd | char\* oldpath | int newdirfd | char\* newpath | int flags |  |
| getrandom | 3 | void\* buf | size\_t buflen | int flags |  |  |  |
| statx | 5 | int dirfd | char\* pathname | int flags | unsigned int mask | struct statx\* statxbuf |  |
| faccessat2 | 4 | int dirfd | char\* pathname | int mode | int flags |  |  |

## Currently Unsupported Syscalls

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
* futex\_requeue
* futex\_wait
* futex\_waitv
* futex\_wake
* futimesat
* get\_mempolicy
* get\_robust\_list
* get\_thread\_area
* getcpu
* getdents
* getitimer
* getpgrp
* getpriority
* getresgid
* getresuid
* getxattr
* getxattrat
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
* listmount
* listxattr
* listxattrat
* llistxattr
* lookup\_dcookie
* lremovexattr
* lsetxattr
* lsm\_get\_self\_attr
* lsm\_list\_modules
* lsm\_set\_self\_attr
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
* mseal
* msgctl
* msgget
* msgrcv
* msgsnd
* munlock
* munlockall
* name\_to\_handle\_at
* open\_by\_handle\_at
* open\_tree
* open\_tree\_attr
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
* preadv
* preadv2
* process\_madvise
* process\_mrelease
* process\_vm\_readv
* process\_vm\_writev
* ptrace
* pwritev
* pwritev2
* quotactl
* quotactl\_fd
* readahead
* reboot
* recvmmsg
* remap\_file\_pages
* removexattr
* removexattrat
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
* settimeofday
* setxattr
* setxattrat
* shmat
* shmctl
* shmdt
* shmget
* signalfd
* signalfd4
* splice
* statmount
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
* uretprobe
* userfaultfd
* ustat
* utime
* utimes
* vfork
* vhangup
* vmsplice
* waitid
