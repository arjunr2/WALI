import re
import pandas as pd
import csv
import argparse
import logging
from pathlib import Path

from typing import List, Dict, Tuple, Any

BASIC_TYPES = ["int", "char", "long", "void"]
# long long in WASM and Native is 64-bits
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

def get_scargs(x):
    """
        System Call arguments processed as basic types
    """
    if not x['# Args']:
        return []
    args = [x["a"+str(i+1)] for i in range(int(x['# Args']))]
    arg_sub = [COMPLEX_TYPES.get(arg, "UNDEF") if arg[-1] != '*' and arg not in BASIC_TYPES \
        else arg for arg in args]
    return arg_sub


def ptr_anonymize(args):
    """
        Anonymize pointer argument types to void*
    """
    return ['void*' if x[-1] == '*' and x[:-1] not in BASIC_TYPES else x for x in args]


def syscall_iter(syscall_info, stub_fn):
    buf = []
    for sc in syscall_info:
        args = get_scargs(sc)
        fn_name = sc['Aliases'] if sc['Aliases'] else sc['Syscall']
        buf.append(stub_fn(sc['NR'], sc['# Args'], sc['Syscall'], fn_name, args))
    return filter(bool, buf)


def gen_and_write(stub_name, syscall_info, outpath):
    """
        Helper method for common stub generation flow
    """
    buf = syscall_iter(syscall_info, stub_name)
    with open(outpath, 'w') as f:
        f.write('\n'.join(buf))

def gen_libc_stubs(spath, syscall_info):
    """
        --------------------------------------------------------------
        Libc WALI: Declarations and Case-statement for syscall-by-number 
        --------------------------------------------------------------
    """
    def def_stub(nr, nargs, name, fn_name, args):
        return "WALI_SYSCALL_DEF ({fn_name}, {arglist});".format(
                    fn_name = fn_name, 
                    arglist = ','.join(ptr_anonymize(args)))
    def case_stub(nr, nargs, name, fn_name, args):
        return "\t\tCASE_SYSCALL ({name}, {fn_name}, {arglist});".format(
            name = name, 
            fn_name = fn_name, 
            arglist = ','.join(['({})a{}'.format(j, i+1) 
                for i, j in enumerate(ptr_anonymize(args))])) if nargs else ""
       
    gen_and_write(def_stub, syscall_info, spath / 'defs.out')
    gen_and_write(case_stub, syscall_info, spath / 'case.out')
    

def gen_wamr_stubs(spath, syscall_info):
    """
        --------------------------------------------------------------
        WAMR WALI: Declarations, Linking Symbols, and Implementation 
        --------------------------------------------------------------
    """
    def declr_stub(nr, nargs, name, fn_name, args):
        return "long wali_syscall_{fn_name} (wasm_exec_env_t exec_env{arglist});".format(
                fn_name = fn_name,
                arglist = ''.join([', long a{}'.format(i+1) for i, j in enumerate(args)]))

    def impl_stub(nr, nargs, name, fn_name, args):
        lines = [f"// {nr} TODO",
                "long wali_syscall_{fn_name} (wasm_exec_env_t exec_env{arglist}) {{".format(
                    fn_name = fn_name, 
                    arglist = ''.join([f", long a{i+1}" for i, j in enumerate(args)])),

                f"\tSC({nr} ,{fn_name});",
                f"\tERRSC({fn_name});",
                "\tRETURN(__syscall{num_args}(SYS_{fn_name}{arglist}));".format(
                    num_args = len(args),
                    fn_name = fn_name,
                    arglist = ''.join([f", a{i+1}" if argty[-1] != '*' else f", MADDR(a{i+1})"
                        for i, argty in enumerate(args)])),
                
                "}\n"
                ] if nargs else [""]
        return '\n'.join(lines)


    def gen_native_args(args):
        return "\"({params}){res}\"".format(
            params = ''.join(["I" if x == "long long"  or x == "long" else "i" for x in args]),
            res = "I"
            )
        
    def symbols_stub(nr, nargs, name, fn_name, args):
        return "\t\t\tNSYMBOL ( {: >20}, {: >30}, {: >12} ),".format(
                    "SYS_" + fn_name,
                    "wali_syscall_" + fn_name,
                    gen_native_args(args)
                ) if nargs else ""

    gen_and_write(declr_stub, syscall_info, spath / 'declr.out')
    gen_and_write(impl_stub, syscall_info, spath / 'impl.out')
    gen_and_write(symbols_stub, syscall_info, spath / 'symbols.out')


def gen_wit_stubs(spath, syscall_info):
    pass

def gen_arch_diff_stubs(spath, syscall_info):
    pass

stub_classes = {
    'libc': gen_libc_stubs,
    'wamr': gen_wamr_stubs,
    'wit': gen_wit_stubs,
    'arch-diff': gen_arch_diff_stubs
}

def parse_args() -> argparse.Namespace:
    """
        Argument parsing for CLI
    """
    parser = argparse.ArgumentParser(prog='wali-autogen', description="Generate WALI descriptions/implementations/stubs for different uses")
    parser.add_argument('--file', '-f', help='CSV file containing syscall format', default='syscall_full_format.csv')
    parser.add_argument('--verbose', '-v', help='Logging verbosity', choices=range(6), type=int, default=4)
    parser.add_argument('stubs', nargs='*', choices=list(stub_classes.keys())+['all'], default='all')
    p = parser.parse_args()
    if p.stubs == 'all' or (type(p.stubs) is list and 'all' in p.stubs):
        p.stubs = stub_classes.keys()
    return p


def parse_csv(filepath: str) -> Tuple[Dict[str, Any], List[str]]:
    df = pd.read_csv(filepath, skiprows=1, keep_default_na=False)
    archs = [k[:-3] for k in df.filter(regex=(".+_NR")).columns]
    dfilter = [x for x in df.to_dict(orient='records') if x['Syscall']]
    logging.debug(dfilter)
    return dfilter, archs
    

def main():
    args = parse_args()
    logging.basicConfig(level=logging.getLevelName((6-args.verbose)*10) if args.verbose != 0 else logging.NOTSET, 
        format='%(levelname)s: %(message)s')
    syscall_info, archs = parse_csv(args.file)
    
    for stub in args.stubs:
        spath = Path(stub)
        logging.info(f"Generating {stub} stubs")
        spath.mkdir(parents=True, exist_ok=True)
        stub_classes[stub](spath, syscall_info)

if __name__ == '__main__':
    main()
