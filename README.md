# Webassembly Linux Interface (WALI)

![WebAssembly Linux Interface](assets/main-logo.png?raw=true)

***A (Nearly-Complete) Linux API for WebAssembly!***

This work is published at *EuroSys 2025* on [**Empowering WebAssembly with Thin Kernel Interfaces**](https://dl.acm.org/doi/abs/10.1145/3689031.3717470).
This repo contains all the compiler and engine prototypes for an implementation of the *WebAssembly Linux Interface* and a brief description of the project's [goals](docs/goals.md). The list of currently supported syscalls can be found [here](docs/support.md)

## Initial Setup

* Clone the repository: `git clone https://github.com/arjunr2/WALI.git` 
* Setup toolchain configs: `python3 toolchains/gen_toolchains.py`

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
See [examples/precompiled](examples/precompiled) for runnable WALI binaries.

#### Native Linux Host
```shell
# Install dependencies
sudo ./apt-install-deps.sh
git submodule update --init wasm-micro-runtime
# Generates `iwasm` symlink in root directory
make iwasm
```

*[Optional, but Recommended] Wasm as a Miscellaneous Binary Format*: By registering Wasm/AoT binaries as a miscellenous binary format with the above engine, `.wasm` files can be executed like ELF files (e.g. `./bash.wasm --norc`).
This is **necessary** to build some [applications](applications) that execute intermediate binaries.
To do this, run:
```shell
# Specify '-p' option to register with systemd-binfmt for reboot survival. Default binfmt_register does not survive system reboots
sudo ./toolchains/binfmt/binfmt_register.sh -p
```

More info on miscellaneous binary formats and troubleshooting can be found [here](https://docs.kernel.org/admin-guide/binfmt-misc.html)



#### Docker Environment for Non-Linux Hosts

```shell
# Building the image
docker build -t wali -f runtime.Dockerfile .
# Running binaries with the image
docker run --rm -it -w /dir -v (pwd):/dir wali <prog.wasm> <args..>
```


### 2. WALI LLVM Toolchain

```shell
git submodule update --init --depth=1 llvm-project
make wali-compiler
```

> **Note**: Building the LLVM suite takes a long time and can consume up to 150GB of disk.


### 3. WALI Sysroot

```shell
git submodule update --init wali-musl
make libc
```

> **Note**: Only the following 64-bit architectures are supported: `x86-64`, `aarch64`, `riscv64`. Future iterations will include a larger set of ISAs.


### 4. AoT Compiler

Generate faster ahead-of time (AoT) compiled executables. For the WAMR 
implementation, additional details can be found on the [WAMR compiler docs](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali/wamr-compiler):
```shell
# Build wamrc
make wamrc
# Using wamrc
wamrc --enable-multi-thread -o <destination-aot-file> <source-wasm-file>
```



## Building WALI Applications

Ensure [initial setup](#initial-setup) is completed. 

> **Note**: For additional information on using/customizing toolchains, see [toolchains](toolchains/README.md) 

**Hello World**

```shell
cd examples
# This script sets up standard build flags for the compiler toolchain.
# For WALI binaries without main/start functions, refer to `print_nostart.c` instead
./compile-wali-standalone.sh -o print.wasm print.c
# Run the binary (or `./print.wasm` if miscellaneous binary format is setup)
../iwasm print.wasm
```

**Unit Tests**
```shell
# WALI executables are located in `./tests/wasm`
make tests
```


## Additional Resources
* [Compiler ports](compiler_ports/README.md) of WALI for other languages.
* [Constraints](docs/constraints.md) of WALI
* [Syscall Information Table](https://docs.google.com/spreadsheets/d/1__2NqMqGLHdjFFYonkF49IkGgfv62TJCpZuXqhXwnlc/edit?usp=sharing) 
* [Zenodo](https://zenodo.org/records/14829424) Ubuntu 22.04 VM artifact for experimenting with WALI
* Related Work: [Verifying System Interfaces Paper](https://cseweb.ucsd.edu/~dstefan/pubs/johnson:2023:wave.pdf)
