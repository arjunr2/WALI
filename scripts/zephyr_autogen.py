import pandas as pd
import argparse
import logging
import json
from pathlib import Path
from typing import List, Dict, Tuple, Any

type_template_file = 'templates/wazi_types.json.template'
sc_all_types = {}

def empty_fn(*args, **kwargs):
    pass

def parse_args() -> argparse.Namespace:
    """
        Argument parsing for CLI
    """
    parser = argparse.ArgumentParser(prog='wazi-autogen', description="Generate WAZI syscall stubs/implementations")
    parser.add_argument('--file', '-f', help='Pickle file containing syscall format', default='pkls/syscalls_zephyr.pkl')
    parser.add_argument('--verbose', '-v', help='Logging verbosity', choices=range(6), type=int, default=4)
    parser.add_argument('--gen-types', '-t', help=f"Generate skeleton of types into \'{type_template_file}\' file", action='store_true')
    return parser.parse_args()


def transform_sc_args(sc_name, sc_args):
    """
        Transform syscall args to basic types
    """
    """
       Arg: {
        ptr_indirection: <int>
        type: <string>
        name: <string>
       }
    """
    for ty, name in sc_args:
        # Remove ZRESTRICT
        tstring = ty[:-len('ZRESTRICT')] if ty.endswith('ZRESTRICT') else ty
        tstring = tstring.rstrip()
        # Remove pointer indirection
        ptr_indirection = 0
        while tstring.endswith('const') or tstring.endswith('*'):
            tstring = tstring[:-len('const')] if tstring.endswith('const') else tstring
            temp = tstring.rstrip('*')
            ptr_indirection += len(tstring) - len(temp)
            tstring = temp.rstrip()
            tstring = tstring[:-len('const')] if tstring.endswith('const') else tstring
            tstring = tstring.rstrip()
        # Remove const
        tstring = tstring[len('const'):] if tstring.startswith('const') else tstring
        tstring = tstring.lstrip()
        if ptr_indirection == 0 and tstring not in sc_all_types:
            logging.info(f"Adding {tstring} for {sc_name}")
            sc_all_types[tstring] = ""

def syscall_iter(df, stub_fn):
    buf = []
    for index, row in df.iterrows():
        args = transform_sc_args(row['name'], row['args'])
        buf.append(stub_fn(index, row['name'], row['args']))
    return filter(bool, buf)

def gen_and_write(df, stub_fn, outpath):
    """
        Helper method for common stub generation flow
    """
    buf = syscall_iter(df, stub_fn)
    with open(outpath, 'w') as f:
        f.write('\n'.join(buf))

def gen_wazi_stubs(spath, df):

    def def_stub(nr, sys_name, args):
        """
            Definitions of C prototypes for WAMR implementation
        """
        pass

    def impl_stub(nr, sys_name, args):
        """
            Auto-implementation of syscalls through simple linear address translations
        """
        lines = [f"// {nr} TODO",
                "long long wazi_syscall_{sys_name} (wasm_exec_env_t exec_env{arglist}) {{".format(
                        sys_name = sys_name,
                        arglist = ''.join([f", long a{i+1}" for i, j in enumerate(args)])),

                f"\tSC({nr} ,{sys_name});",
                f"\tERRSC({sys_name});",
                "\tRETURN({sys_name}({argvals}));".format(
                    num_args = len(args),
                    sys_name = sys_name,
                    argvals = ''.join([f", a{i+1}" if argty[-1] != '*' else f", MADDR(a{i+1})"
                        for i, argty in enumerate(args)])),
                "}\n"
                ]
        return '\n'.join(lines)

    def symbols_stub(nr, sys_name, args):
        """
            WAMR Native Symbols for syscalls
        """
        lines = [
                ]
        return '\n'.join(lines)


    gen_and_write(df, def_stub, spath / 'zephyr_defs.out')
    gen_and_write(df, impl_stub, spath / 'zephyr_impl.out')
    #gen_and_write(df, symbols_stub spath / 'zephyr_symbols.out')


def main():
    args = parse_args()
    logging.basicConfig(level=logging.getLevelName((6-args.verbose)*10) if args.verbose != 0 else logging.NOTSET, 
        format='%(levelname)s: %(message)s')

    df = pd.read_pickle("pkls/syscalls_zephyr.pkl")
    pd.set_option('display.max_rows', None)
    pd.set_option('display.max_colwidth', 120)
    #logging.info(df)
    
    if args.gen_types:
        logging.info("Generating type skeleton")
        """ This will populate sc_all_types """
        syscall_iter(df, empty_fn)
        logging.info(f"Types: {sc_all_types}")
        with open(type_template_file, 'w') as f:
            json.dump(dict(sorted(sc_all_types.items())), f, indent=4)


    spath = Path('wazi')
    spath.mkdir(parents=True, exist_ok=True)
    gen_wazi_stubs(spath, df)


if __name__ == '__main__':
    main()
