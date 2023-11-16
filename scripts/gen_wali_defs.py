import re
import pandas as pd
import csv

BASIC_TYPES = ["int", "char", "long", "void", "..."]
# long long in WASM and Native is 64-bit machine
# Each tuple in complex types denotes (native x64, wasm) size
COMPLEX_TYPES = {
    "off_t": "long long", # "long long",
    "size_t": "unsigned int", # "long", 
    "mode_t": "int",
    "nfds_t": "int",
    "socklen_t": "unsigned int", # "long",
    "clockid_t": "int",
    "uid_t": "int",
    "pid_t": "int",
    "gid_t": "int"
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

            f"\tSC({nr} ,{fn_name});",
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
        params = ''.join(["I" if x == "long long"  or x == "long" else "i" for x in args]),
        res = "I"
        )
    

def gen_native_def(nr, name, fn_name, args):
    return "\t\t\tNSYMBOL ( {: >25}, {: >25}, {: >12} ),".format(
                "__syscall_SYS_" + fn_name,
                "wali_syscall_" + fn_name,
                gen_native_args(args)
            )

def gen_undefined_syscall(nr, name, fn_name, args):
   pass 


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

    #"arch_syscalls_undefined":  (undefined_dict, gen_undefined_syscall, 1)


def main():
    defs = gen_syscall_list()
    
    format_file = "syscall_full_format.csv"
    df = pd.read_csv(format_file, skiprows=1, keep_default_na=False)

    # Find cross-architectures
    archs = [k[:-3] for k in df.filter(regex=(".+_NR")).columns]
    undefined_dict = {arch: [] for arch in archs}

    df_dict = df.to_dict(orient='records')
    syscall_info = df_dict
    
    # Generate syscall lists
    for item in syscall_info:
        if item['Syscall']:
            args, valid = gen_args(item)
            print("{}: {}".format(item['Syscall'], args))
            
            fn_name = item['Aliases'] if item['Aliases'] else item['Syscall']

            for ty, tup in out_dict.items():
                buf, app_fn, ignore_valid = tup
                if valid or ignore_valid:
                    buf.append(app_fn(item['NR'], item['Syscall'], fn_name, args))

            for arch in archs:
                if int(item[f'{arch}_NR']) == -1:
                    undefined_dict[arch].append(fn_name)


    for ty, tup in out_dict.items():
        buf = tup[0]
        with open(ty + ".out", "w") as f:
            f.writelines('\n'.join(buf))

    for arch, buf in undefined_dict.items():
        with open(f"{arch}_syscalls_undefined.out", "w") as f:
            f.writelines('\n'.join(buf))

if __name__ == '__main__':
    main()
