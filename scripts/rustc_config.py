import argparse
from pathlib import Path
import logging
import toml

def create_parser():
    parser = argparse.ArgumentParser(description='Path Argument Parser')
    parser.add_argument('-r', '--rustsrc', type=Path, required=True, help='Path to rust compiler source directory')
    parser.add_argument('-m', '--muslroot', type=Path, required=True, help='Path to musl sysroot directory')
    parser.add_argument('-l', '--llvmbin', type=Path, required=True, help='Path to llvm bin directory')
    return parser

def absresolve(p):
    return str(p.resolve())

# Config.toml update
def update_config_toml(rust, muslroot, llvmbin):
    with open(rust / 'config.toml', 'r') as f:
        config = toml.load(f)

    build_config = {
        'target': ["x86_64-unknown-linux-gnu", "wasm32-wali64-linux-musl"]
    }
    wali_config = {
        'wasm32-wali64-linux-musl': {
            'musl-root': absresolve(muslroot),
            'llvm-config': absresolve(llvmbin / 'llvm-config'),
        },
        'x86_64-unknown-linux-gnu': {
            'llvm-config': absresolve(llvmbin / 'llvm-config'),
        }
    }

    rust_deny = {
        'deny-warnings': False
    }

    if 'build' not in config:
        config['build'] = {}
    config['build'].update(build_config)
    if 'target' not in config:
        config['target'] = {}
    config['target'].update(wali_config)
    if 'rust' not in config:
        config['rust'] = {}
    config['rust'].update(rust_deny)


    logging.info(f"Writing {config} to config.toml")

    with open(rust / 'config.toml', 'w') as f:
        toml.dump(config, f)

# Cargo.toml update
def update_cargo_toml(rust, muslroot, llvmbin):
    with open(rust / 'Cargo.toml', 'r') as f:
        cargo = toml.load(f)
    
    libc_patch = {
        "libc": {
            "git": "https://github.com/arjunr2/rust-libc.git", 
            "branch": "libc-0.2"
        }
    }
    if 'patch' not in cargo:
        cargo['patch'] = {}
    if 'crates-io' not in cargo['patch']:
        cargo['patch']['crates-io'] = {}
    cargo['patch']['crates-io'].update(libc_patch)

    logging.info(f"Writing {cargo} to Cargo.toml")

    with open(rust / 'Cargo.toml', 'w') as f:
        toml.dump(cargo, f)

# Add wasm-ld as the default linker for Cargo.toml
def add_wasm_linker_to_cargo(llvmbin):
    config_path = Path.home() / ".cargo/config.toml"
    wasm_linker = {
        'wasm32-wali64-linux-musl': {
            'linker': absresolve(llvmbin / 'wasm-ld')
        }
    }

    if not config_path.is_file():
        config = { 'target': wasm_linker }
    else:
        with open(config_path, 'r') as f:
            config = toml.load(f)
        if 'target' not in config:
            config['target'] = {}
        config['target'].update(wasm_linker)

    logging.info(f"Writing {config} to {config_path}")

    with open(config_path, 'w') as f:
        toml.dump(config, f)
    

def main():
    logging.basicConfig(level=logging.INFO, 
        format='%(levelname)s: %(message)s')
    parser = create_parser()
    args = parser.parse_args()
    rust, muslroot, llvmbin = args.rustsrc, args.muslroot, args.llvmbin
    update_config_toml(rust, muslroot, llvmbin)
    update_cargo_toml(rust, muslroot, llvmbin)
    add_wasm_linker_to_cargo(llvmbin)


if __name__ == '__main__':
    main()