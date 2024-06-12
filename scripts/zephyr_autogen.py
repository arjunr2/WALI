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
    "bool", "void"
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
COMPLEX_TYPES = {}
JSON_DATA = {}
with open('wazi/wazi_types.json', 'r') as f:
    JSON_DATA = json.load(f)
    for k, v in JSON_DATA.items():
        while v not in BASIC_TYPES:
            v = INTM_TYPES[v] if v in INTM_TYPES else JSON_DATA[v]
        COMPLEX_TYPES[k] = v


BASIC_TYPE_MAP = {**BASIC_TYPES, **INTM_TYPES, **COMPLEX_TYPES}

S = {**JSON_DATA, **INTM_TYPES}
IMPLICIT_PTRS = { k: 1 for k, v in S.items() if v == 'ptr' or v == 'uintptr_t' or k == 'uintptr_t' }

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



def reduce_sc_types(sc_name, sc_args, sc_ret):
    """
        Reduce all system call argument/return types
            Each type has: {
             sc: <string> System call
             ptr_id: <int> Number of pointer indirections
             type: <string> Bare unreduced type
             enum: <bool> Is ENUM?
             name: <string> Name of the variable
             basic_type: <string> Basic reduced type 
            }
    """
    reduce_args = []
    for ty, name in sc_args + [(sc_ret, 'ret')]:
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

    return reduce_args[:-1], reduce_args[-1]


def syscall_iter(df, stub_fn):
    buf = []
    for index, row in df.iterrows():
        args, ret = reduce_sc_types(row['name'], row['args'], row['rettype'])
        logging.debug(args)
        buf.append(stub_fn(index, row['name'], args, ret, row['includefile']))
    return filter(bool, buf)

def gen_and_write(df, stub_fn, outpath):
    """
        Helper method for common stub generation flow
    """
    buf = syscall_iter(df, stub_fn)
    with open(outpath, 'w') as f:
        f.write('\n'.join(buf))

def gen_wazi_stubs(spath, df):

    def declr_stub(nr, sys_name, args, ret, incf):
        """
            Definitions of C prototypes for WAMR implementation
        """
        return "// [{nr}] : {sys_name1} \n{rettype} wazi_syscall_{sys_name2} (wasm_exec_env_t exec_env{arglist});".format(
                    nr = nr,
                    sys_name1 = sys_name,
                    rettype = ret['basic_type'],
                    sys_name2 = sys_name,
                    arglist = ''.join([", {x} sca{y}".format(
                            x = arg['basic_type'], y = idx+1) 
                            for idx, arg in enumerate(args)]
                    )
                )


    def impl_core(sys_name, args, ret):
        def translation(arg, src_name):
            ptr_id = arg['ptr_id']
            # Pointer Translation
            if ptr_id == 1:
                if arg['type'] == 'FILE':
                    return f"({arg['type']}*) MADDR_FILE({src_name})"
                else:
                    return f"({arg['type']}*) MADDR({src_name})"
            elif ptr_id >= 2:
                return "ERROR_PTR"
            # Implicit pointers
            if arg['type'] in IMPLICIT_PTRS:
                return f"({arg['type']}) MADDR({src_name})"
            # Function pointers
            if JSON_DATA.get(arg['type']) == 'fn-ptr':
                return "ERROR_FN_PTR"
            # Struct translation
            if arg['type'] == 'k_timeout_t':
                return "{{ .ticks = {v} }}".format(v=src_name)
            # Union translation, use a field that uses the entire size
            if arg['type'] == 'union fuel_gauge_prop_val':
                return "{{ .chg_current = {v} }}".format(v=src_name)
            if arg['type'] == 'struct log_msg_desc':
                return f"LOG_MSG_DESC_INIT({src_name})"
            if arg['type'] == 'va_list':
                return f"VA_LIST_INIT({src_name})"
            else:
                return src_name

        v = []
        names = []
        for idx, arg in enumerate(args):
            v += ["{ty} {name} = {val};".format(
                    ty = arg['type'],
                    name = "*"*arg['ptr_id'] + arg['name'],
                    val = translation(arg, f"sca{idx+1}")
                )]
            names += [arg['name']]
        v += ["RETURN(({rettype}) {sys_name}({argvals}));".format(
                    rettype = ret['basic_type'],
                    sys_name = sys_name,
                    argvals = ','.join(names))
             ] if ret['basic_type'] != 'void' else [
                 "RETURN_VOID({sys_name}({argvals}));".format(
                    sys_name = sys_name,
                    argvals = ','.join(names))
             ]
        return '\n'.join(['\t' + x for x in v])

    def impl_stub(nr, sys_name, args, ret, incf):
        """
            Auto-implementation of syscalls through simple linear address translations
        """
        lines = [f"// {nr} TODO",
                "{rettype} wazi_syscall_{sys_name} (wasm_exec_env_t exec_env{arglist}) {{".format(
                    rettype = ret['basic_type'],
                    sys_name = sys_name,
                    arglist = ''.join([", {x} sca{y}".format(
                            x = arg['basic_type'], y = idx+1) 
                            for idx, arg in enumerate(args)]
                    )
                ),
                f"\tSC({nr}, {sys_name});",
                f"\tERRSC({sys_name});",
                impl_core(sys_name, args, ret),
                "}\n"
                ]
        return '\n'.join(lines)


    def gen_native_args(args, ret):
        def sym_convert(arg):
            if arg['basic_type'] == "void":
                return ""
            elif arg['basic_type'] == 'int64_t' or arg['basic_type'] == 'uint64_t':
                return "I"
            else:
                return "i"

        return "\"({params}){res}\"".format(
            params = ''.join([sym_convert(x) for x in args]),
            res = sym_convert(ret)
            )

    def symbols_stub(nr, sys_name, args, ret, incf):
        """
            WAMR Native Symbols for syscalls
        """
        return "\tNSYMBOL ( {: >40}, {: >55}, {: >12} ),".format(
                    f"SYS_{sys_name}",
                    f"wazi_syscall_{sys_name}",
                    gen_native_args(args, ret)
                )

    inc_set = set()
    def incs_stub(nr, sys_name, args, ret, incf):
        """
            All include headers necessary for 
        """
        if incf not in inc_set:
            inc_set.add(incf)
            return f"#include <zephyr/drivers/{incf}>"
        else:
            return ""

    gen_and_write(df, declr_stub, spath / 'zephyr_declr.out')
    logging.info("Generated WAZI declarations")
    gen_and_write(df, impl_stub, spath / 'zephyr_impl.out')
    logging.info("Generated WAZI implementation")
    gen_and_write(df, symbols_stub, spath / 'zephyr_symbols.out')
    logging.info("Generated WAZI symbols")
    gen_and_write(df, incs_stub, spath / 'zephyr_incs.out')
    logging.info("Generated WAZI include headers")


def main():
    args = parse_args()
    logging.basicConfig(level=logging.getLevelName((6-args.verbose)*10) if args.verbose != 0 else logging.NOTSET, 
        format='%(levelname)s: %(message)s')

    df = pd.read_pickle("pkls/syscalls_zephyr.pkl")
    pd.set_option('display.max_rows', None)
    pd.set_option('display.max_colwidth', 120)
    #logging.debug(df)
    logging.debug(df['rettype'])
    
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
