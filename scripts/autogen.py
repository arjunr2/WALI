import re
import argparse
import logging
import textwrap
from pathlib import Path
from dataclasses import dataclass, fields
from typing import List, Dict, Any, Callable, Set, Optional, Tuple, Type

# Import definition objects
from syscall_definitions import SYSCALLS, AUX_SYSCALLS, Syscall, AuxSyscall, Nrs, ScArg
from type_system import TypeSystem

@dataclass
class GeneratorContext:
    ts: TypeSystem
    archs: List[str]
    syscalls: Dict[str, Syscall]
    aux_syscalls: Dict[str, AuxSyscall]

    @classmethod
    def create(cls) -> 'GeneratorContext':
        logging.info("Creating context...")
        # Load Type System from wit templates
        script_dir = Path(__file__).parent
        ts = TypeSystem.load(script_dir / 'templates' / 'wit')
        archs = [f.name for f in fields(Nrs)]
        return cls(ts, archs, SYSCALLS, AUX_SYSCALLS)


# --- Base Generator ---
class StubGenerator:
    # Shared paths for all generators
    SCRIPT_DIR = Path(__file__).parent
    TEMPLATES_DIR = SCRIPT_DIR / 'templates'
    
    def __init__(self, spath: Path, context: GeneratorContext):
        self.spath = spath
        self.spath.mkdir(parents=True, exist_ok=True)
        self.ctx = context

    @property
    def ts(self) -> TypeSystem:
        return self.ctx.ts

    @property
    def archs(self) -> List[str]:
        return self.ctx.archs
    
    @property
    def syscalls(self) -> Dict[str, Syscall]:
        return self.ctx.syscalls
    
    @property
    def aux_syscalls(self) -> Dict[str, AuxSyscall]:
        return self.ctx.aux_syscalls

    def generate(self):
        raise NotImplementedError

    def write_file(self, filename: str, content: str):
        """Writes content to file at self.spath / filename."""
        filepath = self.spath / filename
        filepath.parent.mkdir(parents=True, exist_ok=True)
        with open(filepath, 'w') as f:
            f.write(content)

    def gen_and_write(self, stub_fn: Callable, filename: str, include_unimplemented_calls: bool = False, prelude: str = "", postlude: str = ""):
        # Call stubs for each syscall
        buf = [stub_fn(sc) for sc in self.syscalls.values() if include_unimplemented_calls or sc.implemented]
        # Construct content: prelude + joined stubs + postlude
        content_parts = []
        if prelude: content_parts.append(prelude)
        content_parts.append('\n'.join(buf))
        if postlude: content_parts.append(postlude)
        self.write_file(filename, '\n'.join(content_parts))


# --- Specific Generators ---

class LibcGenerator(StubGenerator):
    def generate(self):
        self._gen_c_stubs()
        self._gen_rust_stubs()

    def _ptr_anonymize(self, args: List[ScArg]) -> List[str]:
        """Anonymize pointer argument types to void* for non-basic pointer types."""
        def f(arg: ScArg):
            (indir, base) = arg.ptr_split(max=1)
            return 'void*' if indir > 0 and not base.is_basic_type(self.ts) else arg
        
        return [f(arg) for arg in args]

    def rustify_args(self, sc: Syscall) -> List[str]:
        """Convert argument types to Rust compatible types"""
        def f(arg: ScArg):
            prim = arg.wit_primitive_type(self.ts)
            return prim if not prim.startswith('s') else 'i' + prim[1:]
        
        return [f(x) for x in sc.args_reduce(self.ts)]


    def _gen_c_stubs(self):
        def def_stub(sc: Syscall):
            arglist = ','.join(self._ptr_anonymize(sc.args_reduce(self.ts)))
            s = f"WALI_SYSCALL_DEF ({sc.name}, {arglist});"
            for alias in sc.aliases:
                s += f"\n#define __syscall_SYS_{alias} __syscall_SYS_{sc.name}"
            return s

        def case_stub(sc: Syscall):
            anon_args = self._ptr_anonymize(sc.args_reduce(self.ts))
            arglist = ','.join([f"({typ})a{i+1}" for i, typ in enumerate(anon_args)])
            return f"\t\tCASE_SYSCALL ({sc.name}, {arglist});"

        self.gen_and_write(def_stub, 'defs.out', True)
        self.gen_and_write(case_stub, 'case.out')

    def _gen_rust_stubs(self):
        rpath = self.spath / 'rust'
        rpath.mkdir(parents=True, exist_ok=True)


        def rust_def_stub(sc: Syscall):
            rust_args = self.rustify_args(sc)
            argtys = ', '.join([f"a{i+1}: {typ}" for i, typ in enumerate(rust_args)])
            
            code = textwrap.dedent(f"""\
                /* {sc.nr} */
                #[link_name = "SYS_{sc.name}"]
                pub fn __syscall_SYS_{sc.name}({argtys}) -> ::c_long;""")
            return textwrap.indent(code, '\t')

        def rust_match_stub(sc: Syscall):
            if sc.implemented:
                call_args = ', '.join([f"a{x}" for x, _ in enumerate(sc.args_reduce(self.ts))])
                code = f"super::SYS_{sc.name} => syscall_match_arm!(SYS_{sc.name}, args, {call_args}),"
            else:
                code = f"super::SYS_{sc.name} => unimplemented!(\"WALI syscall '{sc.name}' ({sc.nr}) unimplemented!\"),"
            return textwrap.indent(code, '\t\t')

        def_prelude = textwrap.dedent("""\
            // --- Autogenerated from WALI/scripts/autogen.py ---
            #[link(wasm_import_module = "wali")]
            extern "C" {""")
        def_postlude = "\n}"
        
        # We need a slightly custom writer for rust subdirectory or override gen_and_write
        # But base class gen_and_write writes to self.spath / filename.
        # We can pass 'rust/defs.out' as filename.
        self.gen_and_write(rust_def_stub, 'rust/defs.out', False, def_prelude, def_postlude)

        match_prelude = textwrap.dedent("""\
            #[no_mangle]
            pub unsafe extern "C" fn syscall(num: ::c_long, mut args: ...) -> ::c_long {
                use core::unimplemented;
                match num {""")
        match_postlude = textwrap.dedent("""\
                _ => unimplemented!("WALI syscall number {} out-of-scope!", num),
            }
        }""")
        self.gen_and_write(rust_match_stub, 'rust/match.out', True, match_prelude, match_postlude)


class WamrGenerator(StubGenerator):
    def generate(self):
        def declr_stub(sc: Syscall):
            arglist = ''.join([f", long a{i+1}" for i, _ in enumerate(sc.args_reduce(self.ts))])
            return f"long wali_syscall_{sc.name} (wasm_exec_env_t exec_env{arglist});"

        def impl_stub(sc: Syscall):
            args_red = sc.args_reduce(self.ts)
            arglist_def = ''.join([f", long a{i+1}" for i, _ in enumerate(args_red)])
            
            # Construct return call args
            ret_args = []
            for i, argty in enumerate(args_red):
                if argty.endswith('*'):
                    ret_args.append(f", MADDR(a{i+1})")
                else:
                    ret_args.append(f", a{i+1}")
            ret_arglist = ''.join(ret_args)
            
            return textwrap.dedent(f"""\
                // {sc.nr} TODO
                long wali_syscall_{sc.name} (wasm_exec_env_t exec_env{arglist_def}) {{
                    SC({sc.nr}, {sc.name});
                    ERRSC({sc.name});
                    RETURN(__syscall{len(args_red)}(SYS_{sc.name}{ret_arglist}));
                }}
                """)

        def symbols_stub(sc: Syscall):
            def gen_native_args(args_list):
                params = ''.join(["I" if x.endswith("long long") or x.endswith("long") else "i" for x in args_list])
                return f"\"({params})I\""

            return "\tNSYMBOL ( {: >20}, {: >30}, {: >12} ),".format(
                "SYS_" + sc.name,
                "wali_syscall_" + sc.name,
                gen_native_args(sc.args_reduce(self.ts))
            )

        self.gen_and_write(declr_stub, 'declr.out')
        self.gen_and_write(impl_stub, 'impl.out')
        self.gen_and_write(symbols_stub, 'symbols.out')


class WitGenerator(StubGenerator):
    WIT_KEYWORDS_RESERVED_MAP = {
        "flags": "flags-id",
        "type": "type-id",
        "resource": "resource-id",
        "list": "list-id",
        "option": "option-id",
    }
    
    def generate(self):
        buf = []
        buf_aux = []
        uniq_ptr_types = set()
        aux_ptr_types = set()

        def transform_ptr_arg(arg: ScArg):
            if arg.is_fn_ptr():
                return 'ptr-func'
            arg_no_ptr = arg.rstrip('*')
            ptr_indirection = len(arg) - len(arg_no_ptr)
            return ("ptr-" * ptr_indirection) + arg_no_ptr

        for sc in self.syscalls.values():
            args = [ScArg(x.strip().replace(' ', '-').replace('_', '-')) for x in sc.args]
            args = [transform_ptr_arg(x) for x in args]
            
            up_types = set([x for x in args if x.startswith('ptr-')])
            uniq_ptr_types.update(up_types)

            if sc.implemented:
                l1 = f"\t// [{sc.nr}] {sc.name}({', '.join(sc.args)})"
                wit_args = ', '.join([
                    f"{self.WIT_KEYWORDS_RESERVED_MAP.get(id, id.replace('_', '-'))}: {self.ts.basic_types[arg] if arg in self.ts.basic_types else arg}" 
                    for (id, arg) in zip(sc.args_id, args)
                ])
                l2 = f"\tSYS-{sc.name.replace('_', '-')}: func({wit_args}) -> syscall-result;"
                buf.append(l1 + '\n' + l2)

        for sc in self.aux_syscalls.values():
            args = [x.strip().replace(' ', '-').replace('_', '-') for x in sc.args]
            args = [transform_ptr_arg(ScArg(x)) for x in args]
            
            up_types = set([x for x in args if x.startswith('ptr-')])
            aux_ptr_types.update(up_types)

            l1 = f"\t// {sc.name}({', '.join(sc.args)})"
            wit_args = ', '.join([
                f"{self.WIT_KEYWORDS_RESERVED_MAP.get(id, id.replace('_', '-'))}: {self.ts.basic_types[arg] if arg in self.ts.basic_types else arg}" 
                for (id, arg) in zip(sc.args_id, args)
            ])
            
            name = sc.name.lstrip('_').replace('_', '-')
            start_sig = f"\t{name}: func({wit_args})"
            
            res_ty = None
            if sc.result:
                # If sc.result is basic type, use map
                res_ty = self.ts.basic_types.get(sc.result, sc.result)
            
            if res_ty:
                l2 = f"{start_sig} -> {res_ty};"
            else:
                l2 = f"{start_sig};"
            
            buf_aux.append(l1 + '\n' + l2)

        buf = list(filter(bool, buf))
        buf_aux = list(filter(bool, buf_aux))
        
        self._generate_wit_file(buf, buf_aux, uniq_ptr_types, aux_ptr_types)

    def _generate_wit_file(self, buf, buf_aux, uniq_ptr_types, aux_ptr_types):
        rep = lambda p: p.replace('_', '-').replace(' ', '-')
        comp_types = {'syscall-result': 's64'}
        for k, v in (self.ts.basic_types | self.ts.complex_types).items():
             val = rep(v) if not v.startswith('Array') else v.replace('_', '-')
             comp_types[rep(k)] = val

        type_if = ["interface types {"] + \
                  [f"\ttype {k} = {v};" for k, v in comp_types.items()] + \
                  ["}"]

        # Process Records
        with open(self.TEMPLATES_DIR / 'wit' / 'records.wit.template') as f:
            record_content = f.read()

        record_types = set(re.findall(r'record (\S+)', record_content))
        sc_ptr_types = set()
        
        for x in uniq_ptr_types:
            x_no_ptr = x
            while x_no_ptr.startswith("ptr-"):
                x_no_ptr = x_no_ptr.replace("ptr-", "", 1)
            
            # Helper to check validity
            is_skippable = (x_no_ptr in self.ts.basic_types or 
                            x_no_ptr in comp_types or 
                            x_no_ptr in ['void', 'char'])
            if not is_skippable:
                sc_ptr_types.add(x_no_ptr)

        missing_types = sc_ptr_types.difference(record_types)
        if missing_types:
            logging.warning(f"Missing Records for Complex Types: {missing_types}")
        else:
            logging.info("Successfully bound all records")
        
        sc_prelude = (
            ["interface syscalls {"] +
            [f"\tuse types.{{{', '.join(comp_types)}}};"] +
            ["\t/// Readable pointer types"] +
            [f"\ttype {x} = ptr;" for x in sorted(uniq_ptr_types)] +
            ["", "\t/// Record types"] +
            ["\t" + '\n\t'.join(record_content.split('\n'))] +
            [""]
        )


        aux_prelude = (
            ["interface aux {"] +
            [f"\tuse types.{{{', '.join(comp_types)}}};"] +
            [f"\tuse syscalls.{{{', '.join([x for x in aux_ptr_types if x in uniq_ptr_types])}}};"] +
            [""]
        )

        sc_if = sc_prelude + ["\t/// Syscall methods"] + buf + ["}"]
        aux_if = aux_prelude + ["\t/// Aux methods"] + buf_aux + ["}"]

        with open(self.TEMPLATES_DIR / 'wit' / 'wali.wit.template', 'r') as f:
            template = f.read()

        fill_temp = template.replace(
            '[[TYPES_STUB]]', '\n'.join(type_if)
        ).replace(
            '[[SYSCALLS_STUB]]', '\n'.join(sc_if)
        ).replace(
            '[[AUX_SYSCALLS_STUB]]', '\n'.join(aux_if)
        )

        def matchrep(matchobj):
            num_elem, ty = matchobj.groups()
            return "tuple<{}>".format(','.join([ty] * int(num_elem)))

        # TEMPORARY: No fixed-length arrays in WIT
        fill_temp = re.sub(r'Array\[\s*(\d+),\s*(\S+)\s*\]', matchrep, fill_temp)
        
        self.write_file('wali.wit', fill_temp)


class DocsGenerator(StubGenerator):
    """Generates MkDocs-compatible markdown documentation for WALI syscalls."""
    
    # Docs-specific paths (uses inherited TEMPLATES_DIR)
    DOCS_TEMPLATES_DIR = StubGenerator.TEMPLATES_DIR / 'docs'
    OVERRIDES_FILE = DOCS_TEMPLATES_DIR / 'overrides' / 'syscalls.yaml'
    
    # Badge colors for architectures
    ARCH_COLORS = {
        'x86_64': '4c8cbf',   # Blue
        'ARM64': 'c73e3a',    # Red  
        'RV64': '6b4c9a',     # Purple
    }
    
    def __init__(self, spath: Path, context: GeneratorContext):
        super().__init__(spath, context)
        self.overrides = self._load_overrides()
        self.templates = self._load_templates()
    
    def _load_overrides(self) -> Dict[str, Any]:
        """Load manual documentation overrides from YAML file."""
        import yaml
        if self.OVERRIDES_FILE.exists():
            with open(self.OVERRIDES_FILE) as f:
                return yaml.safe_load(f) or {}
        return {}
    
    def _load_templates(self) -> Dict[str, str]:
        """Load markdown templates."""
        templates = {}
        for name in ['syscall', 'aux', 'specification']:
            tpl_path = self.DOCS_TEMPLATES_DIR / f'{name}.md.template'
            if not tpl_path.exists():
                raise FileNotFoundError(f"Required template not found: {tpl_path}")
            with open(tpl_path) as f:
                templates[name] = f.read()
        return templates
    
    def generate(self):
        """Generate all documentation files."""
        self._gen_specification_index()
        self._gen_syscall_pages()
        self._gen_aux_pages()
        self._copy_static_docs()
    
    def _copy_static_docs(self):
        """Copy static docs (index.md, etc.) to output."""
        import shutil
        src_index = self.DOCS_TEMPLATES_DIR / 'index.md'
        if src_index.exists():
            shutil.copy(src_index, self.spath / 'index.md')
    
    def _make_badge(self, arch: str, number: int) -> str:
        """Generate a Shields.io style badge for syscall number."""
        color = self.ARCH_COLORS.get(arch, '555555')
        if number < 0:
            # Show dash for unsupported architectures
            return f"![{arch}](https://img.shields.io/badge/{arch}----{color})"
        # Using shields.io static badge format
        return f"![{arch}](https://img.shields.io/badge/{arch}-{number}-{color})"
    
    def _make_badges(self, sc: Syscall) -> str:
        """Generate all architecture badges for a syscall."""
        badges = [
            self._make_badge('x86_64', sc.nrs.x86_64),
            self._make_badge('ARM64', sc.nrs.arm64),
            self._make_badge('RV64', sc.nrs.rv64),
        ]
        return " ".join(badges)
    
    def _c_signature(self, name: str, args: List, args_id: List, ret_type: str = "long") -> str:
        """Generate C function signature."""
        args_str = ", ".join([f"{ty} {id}" for ty, id in zip(args, args_id)]) if args else "void"
        return f"{ret_type} {name}({args_str});"
    
    def _wasm_signature(self, name: str, args: List, args_id: List, ret_type: str = "i64", is_syscall: bool = True) -> str:
        """Generate Wasm32 import signature in WAT format."""
        # Map C types to Wasm types
        def to_wasm_type(ty: ScArg) -> str:
            prim = ty.wit_primitive_type(self.ts)
            bitwidth = "32" if int(prim[1:]) < 32 else prim[1:]
            return f"i{bitwidth}"
        
        params = " ".join([f"(param ${id} {to_wasm_type(ty)})" for ty, id in zip(args, args_id)])
        result = f"(result {ret_type})" if ret_type else ""
        
        # Syscalls use SYS_ prefix, aux functions don't
        import_name = f"SYS_{name}" if is_syscall else name
        
        func_body = f"(func {params} {result})".strip()
        return f'(import "wali" "{import_name}" {func_body})'
    
    def _gen_specification_index(self):
        """Generate the specification overview page with syscall table."""
        supp_sc = [s for s in self.syscalls.values() if s.implemented]
        
        # Build syscall table - only name and arch numbers, with SYS_ prefix
        header = "| Syscall | x86_64 | ARM64 | RV64 |"
        sep = "|---------|--------|-------|------|"
        rows = []
        for s in sorted(supp_sc, key=lambda x: x.nrs.x86_64):
            x86 = s.nrs.x86_64 if s.nrs.x86_64 >= 0 else "-"
            arm = s.nrs.arm64 if s.nrs.arm64 >= 0 else "-"
            rv = s.nrs.rv64 if s.nrs.rv64 >= 0 else "-"
            rows.append(f"| [SYS_{s.name}](syscalls/{s.name}.md) | {x86} | {arm} | {rv} |")
        
        syscall_table = "\n".join([header, sep] + rows)
        
        # Build aux table - single column with just names
        aux_header = "| Function |"
        aux_sep = "|----------|"
        aux_rows = []
        for s in self.aux_syscalls.values():
            name_clean = s.name.lstrip('_')
            aux_rows.append(f"| [{s.name}](aux/{name_clean}.md) |")
        
        aux_table = "\n".join([aux_header, aux_sep] + aux_rows)
        
        content = self.templates['specification'].replace(
            '[[NUM_SYSCALLS]]', str(len(supp_sc))
        ).replace(
            '[[NUM_AUX]]', str(len(self.aux_syscalls))
        ).replace(
            '[[SYSCALL_TABLE]]', syscall_table
        ).replace(
            '[[AUX_TABLE]]', aux_table
        )
        
        self.write_file('specification/index.md', content)
    
    def _gen_syscall_pages(self):
        """Generate individual syscall documentation pages."""
        (self.spath / 'specification' / 'syscalls').mkdir(parents=True, exist_ok=True)
        
        for sc in self.syscalls.values():
            if not sc.implemented:
                continue
            self._gen_syscall_page(sc)
    
    def _gen_syscall_page(self, sc: Syscall):
        """Generate a single syscall documentation page."""
        override = self.overrides.get(sc.name, {})
        
        # Build signatures
        c_sig = self._c_signature(sc.name, sc.args, sc.args_id)
        wasm_sig = self._wasm_signature(sc.name, sc.args, sc.args_id)
        
        # Build badges
        badges = self._make_badges(sc)
        
        # Description from override only
        description = override.get('description', '')
        
        # Notes section
        notes = override.get('notes', '')
        notes_md = f"## Notes\n\n{notes}" if notes else ""
        
        content = self.templates['syscall'].replace(
            '[[NAME]]', sc.name
        ).replace(
            '[[BADGES]]', badges
        ).replace(
            '[[DESCRIPTION]]', description
        ).replace(
            '[[C_SIGNATURE]]', c_sig
        ).replace(
            '[[WASM_SIGNATURE]]', wasm_sig
        ).replace(
            '[[NOTES]]', notes_md
        )
        
        self.write_file(f'specification/syscalls/{sc.name}.md', content)
    
    def _gen_aux_pages(self):
        """Generate auxiliary function documentation pages."""
        (self.spath / 'specification' / 'aux').mkdir(parents=True, exist_ok=True)
        
        for aux in self.aux_syscalls.values():
            self._gen_aux_page(aux)
    
    def _gen_aux_page(self, aux: AuxSyscall):
        """Generate a single auxiliary function documentation page."""
        override = self.overrides.get(aux.name, {})
        
        # Build signatures
        ret_type = aux.result if aux.result else "void"
        c_sig = self._c_signature(aux.name, aux.args, aux.args_id, ret_type)
        wasm_ret = "i32" if ret_type in ("int", "unsigned int") else ("i64" if ret_type else "")
        wasm_sig = self._wasm_signature(aux.name, aux.args, aux.args_id, wasm_ret, is_syscall=False)
        
        # Description from override only
        description = override.get('description', '')
        
        # Notes section
        notes = override.get('notes', '')
        notes_md = f"## Notes\n\n{notes}" if notes else ""
        
        name_clean = aux.name.lstrip('_')
        
        content = self.templates['aux'].replace(
            '[[NAME]]', aux.name
        ).replace(
            '[[DESCRIPTION]]', description
        ).replace(
            '[[C_SIGNATURE]]', c_sig
        ).replace(
            '[[WASM_SIGNATURE]]', wasm_sig
        ).replace(
            '[[RETURN_TYPE]]', ret_type
        ).replace(
            '[[NOTES]]', notes_md
        )
        
        self.write_file(f'specification/aux/{name_clean}.md', content)


# --- Registry & Main ---

GENERATORS = {
    'libc': LibcGenerator,
    'wamr': WamrGenerator,
    'wit': WitGenerator,
    'docs': DocsGenerator
}

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog='wali-autogen', 
        description="Generate WALI descriptions/implementations/stubs"
    )
    parser.add_argument('--verbose', '-v', 
                        help='Logging verbosity', 
                        choices=range(6), type=int, default=4)
    parser.add_argument('stubs', 
                        nargs='*', 
                        choices=list(GENERATORS.keys())+['all'], 
                        default='all')
    p = parser.parse_args()
    
    if p.stubs == 'all' or (isinstance(p.stubs, list) and 'all' in p.stubs):
        p.stubs = list(GENERATORS.keys())
    # Ensure list
    if isinstance(p.stubs, str):
        p.stubs = [p.stubs]
        
    return p


def main():
    args = parse_args()
    log_level = logging.getLevelName((6-args.verbose)*10) if args.verbose != 0 else logging.NOTSET
    logging.basicConfig(level=log_level, format='%(levelname)s: %(message)s')

    # Initialize shared context
    ctx = GeneratorContext.create()

    for stub_name in args.stubs:
        if stub_name not in GENERATORS:
            logging.warning(f"Unknown stub type: {stub_name}")
            continue
            
        logging.info(f"Generating {stub_name} stubs")
        
        spath = Path('autogen') / stub_name
        
        # Instantiate and run generator
        GeneratorClass = GENERATORS[stub_name]
        try:
            gen = GeneratorClass(spath, ctx)
            gen.generate()
        except Exception as e:
            logging.error(f"Failed to generate {stub_name}: {e}", exc_info=True)

if __name__ == '__main__':
    main()
