from dataclasses import dataclass, field
from typing import Dict, Set
from pathlib import Path
import re
import logging

# --- Type System & Configuration ---
@dataclass
class TypeSystem:
    basic_types: Dict[str, str] = field(default_factory=dict)
    complex_types: Dict[str, str] = field(default_factory=dict)
    combined_types: Dict[str, str] = field(default_factory=dict)
    wit_primitive_set: Set[str] = field(default_factory=set)

    @classmethod
    def load(cls, templates_dir: Path) -> 'TypeSystem':
        def parse_type_file(path: Path) -> Dict[str, str]:
            prog = re.compile(r'type ([^\s=]+)\s*=\s*(.*);')
            try:
                with open(path) as f:
                    content = f.read()
                return dict(prog.findall(content))
            except FileNotFoundError:
                logging.error(f"Template file not found: {path}")
                return {}

        sub = lambda k, ch: k.replace('-', ch)
        
        primitives_path = templates_dir / 'primitives.wit.template'
        complex_path = templates_dir / 'complex.wit.template'

        raw_basic = parse_type_file(primitives_path)
        basic_types = {sub(k, ' '): v for k, v in raw_basic.items()}
        
        raw_complex = parse_type_file(complex_path)
        complex_types = {
            sub(k, '_'): (sub(v, ' ') if sub(v, ' ') in basic_types else sub(v, '_'))
            for k, v in raw_complex.items()
        }
        
        combined = {**basic_types, **complex_types}
        primitives_set = set(basic_types.values())

        logging.debug(f"Loaded Basic Types: {basic_types}")
        logging.debug(f"Loaded Complex Types: {complex_types}")

        return cls(basic_types, complex_types, combined, primitives_set)
