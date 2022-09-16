from pathlib import Path

#class f(Enum):
#    TIME_PERC = 0
#    SECONDS = 1
#    USECS_PER_CALL = 2
#    CALLS = 3
#    ERRORS = 4
#    SYSCALL = 5

def main():
    directory = Path('strace_bench')
    pathlist = directory.glob('*')

    syscall_set = set()
    for path in pathlist:
        with open(path) as f:
            lines = f.readlines()
            split_lines = [x.split() for x in lines]
            start, end = [i for i, line in enumerate(lines) if line.startswith('-----')]
            
            trace = split_lines[start+1 : end]

            syscalls = [x[-1] for x in trace]
            syscall_set.update(syscalls)

    with open('syscall_list.txt', 'w') as f:
        f.writelines('\n'.join(sorted(syscall_set)))


if __name__ == '__main__':
    main()
