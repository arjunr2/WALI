import pandas as pd
import argparse
import logging
import json
from pathlib import Path
from typing import List, Dict, Tuple, Any

type_template_file = 'templates/wazi_types.json.template'
gensc_all_types = {}

BASIC_TYPE_SET = {
    "int32_t", "uint32_t", "int64_t", "uint64_t",
    "int16_t", "uint16_t", "int8_t", "uint8_t",
    "char", "unsigned char", "int", "unsigned int", 
    "bool"
}

BASIC_TYPES = {x: x for x in BASIC_TYPE_SET}
INTM_TYPES = {
    "ptr": "int32_t",
    "uintptr_t": "uint32_t",
    "long": "int32_t",
    "unsigned long": "uint32_t",
    "size_t": "uint32_t",
    "fn-ptr": "int32_t",
    "va_list": "int32_t"
}

# Simplify all the complex types
with open('wazi/wazi_types.json', 'r') as f:
    COMPLEX_TYPES = json.load(f)
    for k, v in COMPLEX_TYPES.items():
        while v not in BASIC_TYPES:
            v = INTM_TYPES[v] if v in INTM_TYPES else COMPLEX_TYPES
        COMPLEX_TYPES[k] = v


BASIC_TYPE_MAP = {**BASIC_TYPES, **INTM_TYPES, **COMPLEX_TYPES}



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



def reduce_sc_args(sc_name, sc_args):
    """
        Reduce all system call argument types
            Arg: {
             ptr_indirection: <int>
             enum: <bool>
             type: <string>
             name: <string>
            }
    """
    reduce_args = []
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

        is_enum = tstring.startswith('enum')

        # Only used for generation for 'gen_types'
        if ptr_indirection == 0 and not is_enum and tstring not in gensc_all_types:
            logging.debug(f"Adding {tstring} for {sc_name}")
            gensc_all_types[tstring] = ""


        if ptr_indirection != 0:
            basic_type = INTM_TYPES['ptr']
        elif tstring in BASIC_TYPE_MAP:
            basic_type = BASIC_TYPE_MAP[tstring]
        elif is_enum:
            basic_type = 'int32_t'
        else:
            basic_type = "ERROR_TYPE"
        reduce_args.append({
                "sc": sc_name,
                "ptr_id": ptr_indirection,
                "type": tstring,
                "enum": is_enum,
                "name": name,
                "basic_type": basic_type
        })

    return reduce_args


def syscall_iter(df, stub_fn):
    buf = []
    for index, row in df.iterrows():
        args = reduce_sc_args(row['name'], row['args'])
        logging.debug(args)
        buf.append(stub_fn(index, row['name'], args))
    return filter(bool, buf)

def gen_and_write(df, stub_fn, outpath):
    """
        Helper method for common stub generation flow
    """
    buf = syscall_iter(df, stub_fn)
    with open(outpath, 'w') as f:
        f.write('\n'.join(buf))

def gen_wazi_stubs(spath, df):

    def declr_stub(nr, sys_name, args):
        """
            Definitions of C prototypes for WAMR implementation
        """
        return "int64_t wazi_syscall_{sys_name} (wasm_exec_env_t exec_env{arglist});".format(
                    sys_name = sys_name,
                    arglist = ''.join([", {x} a{y}".format(
                            x = arg['basic_type'], y = idx+1) 
                            for idx, arg in enumerate(args)]
                    )
                )


    def impl_core(sys_name, args):
        def translation(arg, src_name):
            ptr_id = arg['ptr_id']
            if ptr_id == 0:
                return src_name
            elif ptr_id == 1:
                if arg['type'] == 'FILE':
                    return f"MADDR_FILE({src_name})"
                else:
                    return f"MADDR({src_name})"
            else:
                return "ERROR_PTR"

        v = []
        names = []
        for idx, arg in enumerate(args):
            v += ["{ty} {name} = {val};".format(
                    ty = arg['type'],
                    name = "*"*arg['ptr_id'] + arg['name'],
                    val = translation(arg, f"a{idx+1}")
                )]
            names += [arg['name']]
        v += ["RETURN({sys_name}({argvals}));".format(
                    sys_name = sys_name,
                    argvals = ','.join(names))
             ]
        return '\n'.join(['\t' + x for x in v])

    def impl_stub(nr, sys_name, args):
        """
            Auto-implementation of syscalls through simple linear address translations
        """
        lines = [f"// {nr} TODO",
                "int64_t wazi_syscall_{sys_name} (wasm_exec_env_t exec_env{arglist}) {{".format(
                    sys_name = sys_name,
                    arglist = ''.join([", {x} a{y}".format(
                            x = arg['basic_type'], y = idx+1) 
                            for idx, arg in enumerate(args)]
                    )
                ),
                f"\tSC({nr}, {sys_name});",
                f"\tERRSC({sys_name});",
                impl_core(sys_name, args),
                "}\n"
                ]
        return '\n'.join(lines)


    def gen_native_args(args):
        return "\"({params}){res}\"".format(
            params = ''.join(["I" if x['basic_type'] == 'int64_t' or x['basic_type'] == 'uint64_t' else "i" for x in args]),
            res = "I"
            )

    def symbols_stub(nr, sys_name, args):
        """
            WAMR Native Symbols for syscalls
        """
        return "\tNSYMBOL ( {: >40}, {: >55}, {: >12} ),".format(
                    f"SYS_{sys_name}",
                    f"wazi_syscall_{sys_name}",
                    gen_native_args(args)
                )



    gen_and_write(df, declr_stub, spath / 'zephyr_declr.out')
    logging.info("Generated WAZI declarations")
    gen_and_write(df, impl_stub, spath / 'zephyr_impl.out')
    logging.info("Generated WAZI implementation")
    gen_and_write(df, symbols_stub, spath / 'zephyr_symbols.out')
    logging.info("Generated WAZI symbols")


def main():
    args = parse_args()
    logging.basicConfig(level=logging.getLevelName((6-args.verbose)*10) if args.verbose != 0 else logging.NOTSET, 
        format='%(levelname)s: %(message)s')

    df = pd.read_pickle("pkls/syscalls_zephyr.pkl")
    pd.set_option('display.max_rows', None)
    pd.set_option('display.max_colwidth', 120)
    logging.debug(df)
    
    if args.gen_types:
        logging.info("Generating type skeleton")
        """ This will populate gensc_all_types """
        syscall_iter(df, empty_fn)
        logging.debug(f"Types: {gensc_all_types}")
        with open(type_template_file, 'w') as f:
            json.dump(dict(sorted(gensc_all_types.items())), f, indent=4)


    spath = Path('wazi')
    spath.mkdir(parents=True, exist_ok=True)
    gen_wazi_stubs(spath, df)


if __name__ == '__main__':
    main()
