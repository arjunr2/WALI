# Webassembly Linux Interface (WALI)

![WebAssembly Linux Interface](assets/main-logo.png?raw=true)

***A (Nearly-Complete) Linux API for WebAssembly!***

This is a result of work published at *EuroSys 2025* on [**Empowering WebAssembly with Thin Kernel Interfaces**](https://dl.acm.org/doi/abs/10.1145/3689031.3717470) (arxiv version available [here](https://arxiv.org/abs/2312.03858))

This repo contains all the compiler and engine prototypes for an implementation of the *WebAssembly Linux Interface*. A list of supported syscalls can be found [here](docs/support.md)

## Initial Setup

Setup toolchain configs: `python3 toolchains/gen_toolchains.py`

## Component Setup

There are four major toolchain components, that may be incrementally built:

***I just want to run WALI Wasm executables!***:
1. [WALI Engine](#1-wali-engine)

***I want to compile/build WALI executables!***:

2. [WALI LLVM Toolchain](#2-wali-llvm-toolchain)
3. [WALI Sysroot](#3-wali-sysroot)

***I want to AoT compile the Wasm executables to go fast!***

4. [AoT Compiler](#4-aot-compiler)


### 1. WALI Engine

We include a baseline implementation in WAMR. 
See [examples/precompiled](examples/precompiled) after building for runnable WALI binaries.

#### Native Linux Host

Install dependencies with `sudo ./apt-install-deps.sh` or equivalent packages. Then build with:
```shell
git submodule update --init wasm-micro-runtime
# Generates `iwasm` symlink in root directory
make iwasm
```

##### WASM as a Miscellaneous Binary Format (Optional but Recommended)

WALI Wasm/AoT binaries can be executed like ELF files with `iwasm` (e.g. `./bash.wasm --norc`).
This is recommended since it simplifies execution and is **necessary** to build some [applications](applications) out-of-the-box.
To do this, run:

```shell
cd misc
source gen_iwasm_wrapper.sh
# Specify '-p' option to register with systemd-binfmt for reboot survival. Default binfmt_register does not survive system reboots
sudo ./binfmt_register.sh -p
```

For more info about miscellaneous binary formats and troubleshooting, see [here](https://docs.kernel.org/admin-guide/binfmt-misc.html)


#### Non-Linux Host (Docker Environment)

To build the image:
```shell
docker build -t wali -f runtime.Dockerfile .
```

You can then run WALI binaries with the image:
```shell
docker run --rm -it -w /dir -v (pwd):/dir wali <prog.wasm> <args..>
```


### 2. WALI LLVM Toolchain

```shell
git submodule update --init --depth=1 llvm-project
make wali-compiler
```

**NOTE**: Building the LLVM suite takes a long time and can consume up to 150GB of disk. The compiler is essential if you want to rebuild libc or build applications.


### 3. WALI Sysroot

```shell
git submodule update --init wali-musl
make libc
```

**NOTE**: Only the following 64-bit architectures are supported: `x86-64`, `aarch64`, `riscv64`. Future iterations will include a larger set of ISAs.


### 4. AoT Compiler

Generates faster ahead-of time compiled executables. For the WAMR implementation, build with:
```
make wamrc
```

The `wamrc` symlink can be used as follows:

```shell
# --enable-multi-thread flag is needed for thread support
wamrc --enable-multi-thread -o <destination-aot-file> <source-wasm-file>
```

Refer to [WAMR compiler docs](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali/wamr-compiler) for more info.


## Building WALI Applications

Ensure [initial setup](#initial-setup) is completed. 

> For additional information on using/customizing toolchains, see [toolchains](toolchains/README.md) 

### Hello World

You can use the convenience bash script in the [examples](examples) directory. For instance:
```shell
cd examples
./compile-wali-standalone.sh -o printf.wasm printf.c
# Run the binary. Can just run `printf.wasm` if miscellaneous binary format is setup
../iwasm printf.wasm
```

### Building C Tests
```shell
# WALI executables are located in `./tests/wasm`
make tests
```

## Compiler Ports

### Rust

> **Note**: Preliminary support for a [`wasm32-wali-linux-musl`](https://doc.rust-lang.org/nightly/rustc/platform-support/wasm32-wali-linux.html) target has been upstreamed to rustc! 
> Support for this target requires several ecosystem components to ratchet up to something usable for long-term stability, and may currently be broken. 
> Use the below out-of-tree build process for rustc if you need a stable target as a proof-of-concept.

We support a custom Rust compiler with a `wasm32-wali-linux-musl` target. 
Existing `cargo` and  `rustup` are required for a successful build.
To build `rustc`, run:

```shell
make rustc
```

This adds a new toolchain to `rustup` named `wali` with the new target.
To compile applications:
```shell
cargo +wali build --target=wasm32-wali-linux-musl
```

> **Note**: Many applications will currently require a custom [libc](https://github.com/arjunr2/rust-libc.git) to
be patched into `Cargo.toml` for the out-of-tree build.



## Project Motivation
The WALI for WebAssembly aims to push lightweight virtualization
down to prevalent, low-level Linux applications. 
WALI adopts a layering approach to API design, allowing WASI (and other arbitrary Wasm APIs) to be virtualized over it, 
establishing infrastructure for Wasm both in research and industry.

Building and running Wasm binaries is now **trivial** with WALI, while improving ecosystem security by layering Wasm APIs

> Wasm possesses different runtime properties than some lower level languages like C (type-safety, sandboxing, etc.). The operation of WALI on these applications may differ as listed [here](docs/constraints.md)

## Resources
* [Zenodo](https://zenodo.org/records/14829424) Ubuntu 22.04 VM artifact for experimenting with WALI
* [Syscall Information Table](https://docs.google.com/spreadsheets/d/1__2NqMqGLHdjFFYonkF49IkGgfv62TJCpZuXqhXwnlc/edit?usp=sharing)
* Related Work: [Verifying System Interfaces Paper](https://cseweb.ucsd.edu/~dstefan/pubs/johnson:2023:wave.pdf)
