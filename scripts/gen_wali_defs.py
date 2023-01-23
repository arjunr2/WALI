import re
import pandas as pd
import csv

BASIC_TYPES = ["int", "char", "long", "void", "..."]
COMPLEX_TYPES = {
    "off_t": "long long",
    "size_t": "long", 
    "mode_t": "int",
    "nfds_t": "int",
    "socklen_t": "long",
    "clockid_t": "int",
    "uid_t": "int",
    "pid_t": "int",
    "gid_t": "int"
}

# long long in WASM is really a long in 64-bit machine
WASM_TO_NATIVE_SIZES = {
    "long long": "long"
}


def gen_syscall_list():
    input_file = "syscallnrs/x86-64.h"
    out_file = "syscall_list.txt"
    defs = []

    with open(input_file, "r") as f:
        pattern = re.compile(r"__NR_(\S*)");
        matches = [pattern.search(line) for line in f.readlines()]
        defs = [match.group(1) for match in matches if match is not None]
            
    with open(out_file, "w") as f:
        f.writelines([x+'\n' for x in defs])
    
    return defs


def gen_args(x):
    if not x['# Args']:
        return [], False

    args = [x["a"+str(i+1)] for i in range(int(x['# Args']))]
    arg_sub = []
    for arg in args:
        if arg[-1] == '*': 
            arg_sub.append(arg[:-1]+'*' if arg[:-1] in BASIC_TYPES else "void*")
        elif arg not in BASIC_TYPES:
            arg_sub.append(COMPLEX_TYPES.get(arg, "UNDEF"))
        else:
            arg_sub.append(arg)
    
    return arg_sub, True



def gen_syscall_def(nr, name, fn_name, args):
    return f"WALI_SYSCALL_DEF ({fn_name}, {','.join(args)});"

def gen_case(nr, name, fn_name, args):
    return "\t\tCASE_SYSCALL ({name}, {fn_name}, {arglist});".format(
            name = name, 
            fn_name = fn_name, 
            arglist = ','.join( ['({})a{}'.format(j, i+1) \
                if j != '...' else 'a{}'.format(i+1) for i, j in enumerate(args)]))

def gen_declr(nr, name, fn_name, args):
    return "long wali_syscall_{fn_name} (wasm_exec_env_t exec_env{arglist});".format(
            fn_name = fn_name,
            arglist = ''.join([', long a{}'.format(i+1) for i, j in enumerate(args)]))

def gen_base_impl(nr, name, fn_name, args):
    lines = [f"// {nr} TODO",
            "long wali_syscall_{fn_name} (wasm_exec_env_t exec_env{arglist}) {{".format(
                fn_name = fn_name, 
                arglist = ''.join([f", long a{i+1}" for i, j in enumerate(args)])),

            f"\tSC({fn_name});",
            f"\tERRSC({fn_name});",
            "\treturn __syscall{num_args}(SYS_{fn_name}{arglist});".format(
                num_args = len(args),
                fn_name = fn_name,
                arglist = ''.join([f", a{i+1}" if argty[-1] != '*' else f", MADDR(a{i+1})"
                    for i, argty in enumerate(args)])),
            
            "}\n"
            ]
    return '\n'.join(lines)


def gen_native_args(args):
    return "\"({params}){res}\"".format(
        params = ''.join(["I" if x == "long long" else "i" for x in args]),
        res = "i"
        )
    

def gen_native_def(nr, name, fn_name, args):
    return "\t\t\tNSYMBOL ( {: >25}, {: >25}, {: >12} ),".format(
                "__syscall_SYS_" + fn_name,
                "wali_syscall_" + fn_name,
                gen_native_args(args)
            )


wali_def_list = []
case_list = []
declr_list = []
impl_list = []
native_list = []

out_dict = {
    "wali_syscall_defs":    (wali_def_list, gen_syscall_def, 1),
    "wali_syscall_cases":       (case_list, gen_case, 0),
    "wali_syscall_declr":      (declr_list, gen_declr, 1),
    "wali_syscall_impl":       (impl_list, gen_base_impl, 0),
    "wali_syscall_native":      (native_list, gen_native_def, 0)
}


def main():
    defs = gen_syscall_list()
    
    format_file = "syscall_format.csv"
    df = pd.read_csv(format_file, skiprows=1, keep_default_na=False)
    df_dict = df.to_dict(orient='records')

    syscall_info = df_dict
    
    for item in syscall_info:
        args, valid = gen_args(item)
        print("{}: {}".format(item['Syscall'], args))
        
        fn_name = item['Aliases'] if item['Aliases'] else item['Syscall']

        for ty, tup in out_dict.items():
            buf, app_fn, ignore_valid = tup
            if valid or ignore_valid:
                buf.append(app_fn(item['NR'], item['Syscall'], fn_name, args))


    for ty, tup in out_dict.items():
        buf = tup[0]
        with open(ty + ".out", "w") as f:
            f.writelines('\n'.join(buf))

if __name__ == '__main__':
    main()
