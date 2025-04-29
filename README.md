# Webassembly Linux Interface (WALI)

![WebAssembly Linux Interface](assets/main-logo.png?raw=true)

This is a result of work published at *EuroSys 2025* on [**Empowering WebAssembly with Thin Kernel Interfaces**](https://dl.acm.org/doi/abs/10.1145/3689031.3717470) (arxiv version available [here](https://arxiv.org/abs/2312.03858))

This repo contains all the compiler and engine prototypes for an implementation of the *WebAssembly Linux Interface*. A list of supported syscalls can be found [here](docs/support.md)

## Overview
WALI is a complete(ish) abstraction over Linux for WebAssembly that aims to push lightweight virtualization
down to prevalent, low-level Linux applications. 
WALI adopts a layering approach to API design, allowing WASI (and other arbitrary Wasm APIs) to be virtualized over it, 
establishing infrastructure for Wasm both in research and industry.

Building and running Wasm binaries is now **trivial** with WALI, while improving ecosystem security

## Component Setup

Before proceeding, make sure all dependencies are installed with `sudo ./apt-install-deps.sh`. 
There are four major toolchain components, that may be incrementally built based on requirements:

***I just want to run WALI apps!***:
1. [WALI Engine](#1-wali-engine)

***I want to compile/build WALI apps!***:

2. [WALI LLVM Toolchain](#2-wali-llvm-toolchain)
3. [WALI Sysroot](#3-wali-sysroot)

***I want to AoT compile WALI apps to go fast!***

4. [AoT Compiler](#4-aot-compiler)


### 1. WALI Engine

We include a baseline implementation in WAMR. To build:
```shell
git submodule update --init wasm-micro-runtime
make iwasm
```
An `iwasm` symlink executable should be generated in the root directory that can execute WALI binaries (e.g. `./iwasm -v=0 --stack-size=524288 <path-to-wasm-file>`).
See [Sample Applications](#sample-applications) for test binaries.


#### WASM as a Miscellaneous Binary Format!

WALI Wasm/AoT binaries can be executed like ELF files with `iwasm` (e.g. `./bash.wasm --norc`)!
This will simplify all WALI toolchain builds and is **required** to compile some [applications](applications) in our repo.
To do this, run:

```shell
cd misc
source gen_iwasm_wrapper.sh
# Default binfmt_register does not survive reboots in the system
# Specify '-p' option to register with systemd-binfmt for reboot survival
sudo ./binfmt_register.sh -p
```

More information about miscellaneous binary formats and troubleshooting can be found [here](https://docs.kernel.org/admin-guide/binfmt-misc.html)


### 2. WALI LLVM Toolchain

```shell
git submodule update --init llvm-project
make wali-compiler
```

**NOTE**: Building the LLVM suite takes a long time and can consume up to 150GB of disk. The compiler is essential if you want to rebuild libc or build applications.


### 3. WALI Sysroot

To build libc:
```shell
git submodule update --init wali-musl
make libc
```

We currently support 64-bit architectures (x86-64, aarch64, riscv64) with hopes to expand
to more architectures. 


### 4. AoT Compiler

Generates faster ahead-of time compiled executables. For our WAMR implementation, build as:
```
make wamrc
```

Refer to [WAMR compiler](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali/wamr-compiler)
for any extra information on the build.
Once completed, a symlink to `wamrc` is generated in the root directory:
```shell
wamrc --enable-multi-thread -o <destination-aot-file> <source-wasm-file>  # We require --enable-multi-thread flag for threads
```


## Adapting Build Systems to WALI

We provide three configuration files with toolchain requirements, drastically easing plug-in into major builds
1. Bash: Source the [wali\_config.sh](wali_config.sh) (see [tests/compile-wali.sh](tests/compile-wali.sh))
2. Make: Include [wali\_config.mk](wali_config.mk) (see [applications/Makefile](applications/Makefile))
3. CMake: The [wali\_config\_toolchain.cmake](wali_config_toolchain.cmake) file can be used directly in `CMAKE\_TOOLCHAIN\_FILE`


## Sample Applications

* **Tests** can be built with `make tests`. WALI executables are located in `tests/wasm` -- corresponding native ELF files in `tests/elf` can be used to compare against the WASM output
* **Apps**: The [sample-apps](sample-apps) directory has few several popular prebuilt binaries to run


## Compiler Ports

### Rust

> **Note**: Preliminary support for a [`wasm32-wali-linux-musl`](https://doc.rust-lang.org/nightly/rustc/platform-support/wasm32-wali-linux.html) target has been upstreamed to rustc! 
> Support for this target requires several ecosystem components to ratchet up to something usable long-term stability, and may hence currently be broken. 
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



## Resources
* Wasm possesses different runtime properties than some lower level languages like C (type-safety, sandboxing, etc.). The operation of WALI on these applications may differ as listed [here](docs/constraints.md)
* [Zenodo](https://zenodo.org/records/14829424) Ubuntu 22.04 VM artifact for experimenting with WALI
* [Syscall Information Table](https://docs.google.com/spreadsheets/d/1__2NqMqGLHdjFFYonkF49IkGgfv62TJCpZuXqhXwnlc/edit?usp=sharing)
* Related Work: [Verifying System Interfaces Paper](https://cseweb.ucsd.edu/~dstefan/pubs/johnson:2023:wave.pdf)

