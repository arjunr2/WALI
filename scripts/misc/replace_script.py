import re

def main():
    input_file = open("chmod.c", "r").read()
    print(input_file)
    
    pattern = re.compile(r"syscall\s*\(([^,]*),\s*(.*)\)");

    replaced_file = pattern.sub(r"syscall_\1(\2)", input_file)

    print("\n -- New file --")
    print(replaced_file)

if __name__ == '__main__':
    main()
