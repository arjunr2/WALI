# Webassembly Linux Interface (WALI)

![WebAssembly Linux Interface](assets/main-logo.png?raw=true)

This repo serves to prototype an implementation of the [WebAssembly Linux Interface](https://arxiv.org/abs/2312.03858). For current range of 
support, refer [here](docs/support.md)

## Overview
WALI is a complete(ish?) abstraction over Linux for WebAssembly that aims to push lightweight virtualization
down to even low-level system applications. 
WALI adopts a layering approach to API design, establishing infrastructure for facilitating WebAssembly-oriented research 
by virtualizing high-level APIs and seamless build-run-deploy workflows of arbitrary applications.
We create a custom modified C standard library ([musl libc](https://github.com/arjunr2/wali-musl)) that uses WALI
and produce a baseline implementation in [WAMR](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali)


## Component Setup

Before proceeding, make sure all dependencies are installed with `sudo ./apt-install-deps.sh`. 
There are four major toolchain components, that may be incrementally built based on requirements:

*I just want to run WALI apps!*:
1. [WALI runtime](#wali-runtime)

*I want to compile/build WALI apps!*:

2. [Clang compiler](#wali-llvm-compiler)
3. [WALI Sysroot](#wali-sysroot)

*I want to AoT compile WALI apps to go fast!*

4. [AoT Compiler](#aot-compiler)


### WALI runtime

We have a baseline implementation in [WAMR](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali). To build:
```shell
make iwasm
```
An `iwasm` symlink executable should be generated in the root directory that can execute WALI binaries (e.g. `./iwasm -v=0 --stack-size=524288 <path-to-wasm-file>`).

**Highly Recommended Step**: Register Wasm as a [miscellaneous binary format](#wasm-as-a-miscellaneous-binary-format).
See [Sample Applications](#sample-applications) for test binaries.


### WALI LLVM compiler

```shell
make wali-compiler
```

**NOTE**: Building the LLVM suite takes a long time and can consume up to 150GB of disk. The compiler is essential if you want to rebuild libc or build applications.


### WALI Sysroot

To build libc:
```shell
make libc
```

We currently support 64-bit architectures for x86-64, aarch64, and riscv64, with hopes to expand
to more architectures. 


### AoT Compiler

Generates faster ahead-of time compiled executables. For our WAMR implementation, build as:
```
make wamrc
```

Refer to [WAMR compiler](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/a29e5c633c26a30e54373f658394fab2b95f394e/wamr-compiler)
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
We support a custom Rust compiler with a `wasm32-wali-linux-musl` target. 
Existing `cargo` and  `rustup` and required for a successful build.
To build `rustc`, run:

```shell
make rustc
```

This adds a new toolchain to `rustup` named `wali` with the new target.
To compile applications:
```shell
cargo +wali build --target=wasm32-wali-linux-musl
```

**NOTE**: Many applications will currently require a custom [libc](https://github.com/arjunr2/rust-libc.git) to
be patched into `Cargo.toml` until potential upstreaming is possible.


## WASM as a Miscellaneous Binary Format!

This will **greatly** simplify all toolchain builds for WALI out-of-the-box.
It is **required** to compile some [applications](applications) in our repo
```shell
cd misc
source gen_iwasm_wrapper.sh
# Default binfmt_register does not survive reboots in the system
# Specify '-p' option to register with systemd-binfmt for reboot survival
sudo ./binfmt_register.sh -p
```

This points Linux to `iwasm` for invoking any WASM/AoT files just like ELF files! (e.g. `./bash.wasm --norc`). 
More information about miscellaneous binary formats and troubleshooting can be found [here](https://docs.kernel.org/admin-guide/binfmt-misc.html)


## Resources
* Wasm possesses different runtime properties than some lower level languages like C (type-safety, sandboxing, etc.). The operation of WALI on these applications may differ as listed [here](docs/constraints.md)
* [Syscall Information Table](https://docs.google.com/spreadsheets/d/1__2NqMqGLHdjFFYonkF49IkGgfv62TJCpZuXqhXwnlc/edit?usp=sharing)
* This [paper](https://cseweb.ucsd.edu/~dstefan/pubs/johnson:2023:wave.pdf) and its related work section, especially the bit labeled "Modeling and verifying system interfaces"

