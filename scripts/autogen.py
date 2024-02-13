import re
import pandas as pd
import csv
import argparse
import logging
import system_calls
from pathlib import Path

from typing import List, Dict, Tuple, Any

# Initialize primitive/complex types from template
##########################
def parse_type_file(path):
    prog = re.compile(r'type ([^\s=]+)\s*=\s*(.*);')
    with open(path) as f:
        content = f.read()
    return dict(prog.findall(content))

sub = lambda k, ch: k.replace('-', ch)

BASIC_TYPES = { sub(k, ' '): v for k, v in parse_type_file('templates/primitives.wit.template').items() }
COMPLEX_TYPES = { sub(k, '_'): sub(v, ' ') if sub(v, ' ') in BASIC_TYPES else sub(v, '_') 
                    for k, v in parse_type_file('templates/complex.wit.template').items()}
##########################



def get_scargs(x, replace_complex: bool = True):
    """
        System Call arguments processed as basic types
    """
    if not x['# Args']:
        return []
    args = [x["a"+str(i+1)] for i in range(int(x['# Args']))]
    arg_sub = [COMPLEX_TYPES.get(arg, "UNDEF") if arg[-1] != '*' and arg not in BASIC_TYPES \
        else arg for arg in args] if replace_complex else args
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

def gen_libc_stubs(spath, syscall_info, archs):
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
    

def gen_wamr_stubs(spath, syscall_info, archs):
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
        return "\tNSYMBOL ( {: >20}, {: >30}, {: >12} ),".format(
                    "SYS_" + fn_name,
                    "wali_syscall_" + fn_name,
                    gen_native_args(args)
                ) if nargs else ""

    gen_and_write(declr_stub, syscall_info, spath / 'declr.out')
    gen_and_write(impl_stub, syscall_info, spath / 'impl.out')
    gen_and_write(symbols_stub, syscall_info, spath / 'symbols.out')


def gen_wit_stubs(spath, syscall_info, archs):
    """
        --------------------------------------------------------------
        WIT WALI Syscall Interface Declaration
        --------------------------------------------------------------
    """
    # Outputs
    buf = []
    uniq_ptr_types = set()

    def wit_sc_def(nr, nargs, fn_name, args, orig_args):
        l1 = "\t// [{nr}] {fn_name}({orig_args})".format(
            nr = nr,
            fn_name = fn_name,
            orig_args = ', '.join(orig_args))
        l2 = "\tSYS-{fn_name}: func({arglist}) -> syscall-result;".format(
            fn_name = fn_name.replace('_', '-'),
            arglist = ', '.join(["a{}: {}".format(
                        i+1, BASIC_TYPES[arg] if arg in BASIC_TYPES else arg) 
                        for i, arg in enumerate(args)])
        ) 
        return '\n'.join([l1, l2]) if nargs else ""

    for sc in syscall_info:
        orig_args = get_scargs(sc, False)
        args = [x.strip().replace(' ', '-').replace('_', '-') for x in orig_args]
        args = [f"ptr-{x[:-1]}" if x[-1] == '*' else x for x in args]
        up_types = set([x for x in args if x.startswith('ptr-')])
        fn_name = sc['Aliases'] if sc['Aliases'] else sc['Syscall']
        uniq_ptr_types.update(up_types)
        buf.append(wit_sc_def(sc['NR'], sc['# Args'], fn_name, args, orig_args))

    buf = list(filter(bool, buf))


    ### Generate types and syscall interface
    comp_types = {
        'syscall-result': 's64',
        **{k.replace('_', '-'): v for k, v in COMPLEX_TYPES.items()}
    }
    type_if = ["interface types {"] + ["\ttype {} = {};".format(k, BASIC_TYPES[v] if v in BASIC_TYPES else v) 
                for k, v in comp_types.items()] + \
            ["}"]

    # Match set of record bindings for complex pointer types used in syscalls
    with open('templates/records.wit.template') as f:
        record_content = f.read()
    record_types = set(re.findall(r'record (\S+)', record_content))
    sc_ptr_types = set([x[4:] for x in uniq_ptr_types if x.startswith('ptr-') and \
            x[4:] not in BASIC_TYPES and x[4:] not in comp_types \
            and x[4:] != 'void' and x[4:] != 'char'])
    logging.warning(f"Missing Records for Complex Types: {sc_ptr_types.difference(record_types)}")
    
    sc_prelude = ["interface syscalls {"] + \
                ["\tuse types.{{{}}};".format(', '.join([k for k in comp_types]))] + \
                ["\t/// Readable pointer types"] + \
                ["\ttype {} = ptr;".format(x) for x in sorted(uniq_ptr_types)] + \
                [""] + \
                ["\t/// Record types"] + \
                ["\t" + '\n\t'.join(record_content.split('\n'))] + \
                [""]

    
    sc_if = sc_prelude + ["\t/// Syscall methods"] + buf + ["}"]

    # Fill in template
    with open('templates/wali.wit.template', 'r') as f:
        template = f.read()

    fill_temp = template.replace(
        '[[TYPES_STUB]]', '\n'.join(type_if)
        ).replace(
        '[[SYSCALLS_STUB]]', '\n'.join(sc_if))

    with open(spath / 'wali.wit',  'w') as f:
        f.write(fill_temp)



def gen_arch_diff_stubs(spath, syscall_info, archs):
    """
        --------------------------------------------------------------
        Cross-Architecture syscall differences
        --------------------------------------------------------------
    """
    bufs = {x: [] for x in archs}
    for sc in syscall_info:
        for arch in archs:
            if sc[f"{arch}_NR"] and int(sc[f"{arch}_NR"]) == -1:
                bufs[arch].append(sc["Syscall"])
    for arch, buf in bufs.items():
        with open(spath / f"{arch}_undefined.out", 'w') as f:
            f.write('\n'.join(buf))


def gen_markdown_stubs(spath, syscall_info, archs):
    """
        --------------------------------------------------------------
        Markdown tracking syscall support
        --------------------------------------------------------------
    """
    syscalls = system_calls.syscalls()
    arch_supp = archs + ['x86_64']
    arch_remap = {
        'aarch64': 'arm64',
    }
    arch_supp = {arch_remap[x] if x in arch_remap else x for x in arch_supp}
    arch_calls = [set(syscalls.syscalls['archs'][v].keys()) for v in arch_supp]
    arch_call_set = set().union(*arch_calls)

    df = pd.DataFrame.from_dict(syscall_info)
    # Get supported and unsupported set
    supp_set = set([s['Syscall'] for s in syscall_info if s['# Args']])
    supp_df = df[df['Syscall'].isin(supp_set)]
    supp_format_df = supp_df[['Syscall', '# Args', *[f"a{x+1}" for x in range(6)]]]

    unsupp_set = arch_call_set.difference(supp_set)
    unsupp_list = ["* {}".format(x) for x in sorted(unsupp_set)]

    # Fill in template
    with open('templates/support.md.template', 'r') as f:
        template = f.read()

    fill_temp = template.replace(
        '[[NUM_SUPPORTED_SYSCALLS_STUB]]', 
        str(len(supp_format_df))
        ).replace(
        '[[SUPPORTED_SYSCALLS_STUB]]', 
        supp_format_df.to_markdown(index=False).replace('*', '\*').replace('_', '\_')
        ).replace(
        '[[UNSUPPORTED_SYSCALLS_STUB]]', '\n'.join(unsupp_list).replace('_', '\_'))

    with open(spath / 'support.md', 'w') as f:
        f.write(fill_temp)
    


stub_classes = {
    'libc': gen_libc_stubs,
    'wamr': gen_wamr_stubs,
    'wit': gen_wit_stubs,
    'arch-diff': gen_arch_diff_stubs,
    'markdown': gen_markdown_stubs
}

def parse_args() -> argparse.Namespace:
    """
        Argument parsing for CLI
    """
    parser = argparse.ArgumentParser(prog='wali-autogen', description="Generate WALI descriptions/implementations/stubs for different uses")
    parser.add_argument('--file', '-f', help='CSV file containing syscall format', default='csvs/syscall_full_format.csv')
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
    
    logging.info(f"Basic Types: {BASIC_TYPES}")
    logging.info(f"Complex Types: {COMPLEX_TYPES}")
    for stub in args.stubs:
        spath = Path(stub)
        logging.info(f"Generating {stub} stubs")
        spath.mkdir(parents=True, exist_ok=True)
        stub_classes[stub](spath, syscall_info, archs)

if __name__ == '__main__':
    main()
