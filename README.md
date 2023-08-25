# Webassembly Linux Interface (WALI)

This repo serves to prototype an implementation of the WebAssembly Linux Interface.

## Overview
WALI is a complete(ish?) abstraction over Linux for WebAssembly that aims to push lightweight virtualization
down to even low-level system applications. 
A parallel goal is to increase the scope for WebAssembly-oriented research by building the infrastructure for 
seamless build-run-deploy workflows of WebAssembly applications.
We create a custom modified C standard library ([musl libc](https://github.com/arjunr2/wali-musl)) that uses WALI
and produce a baseline implementation in [WAMR](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali)

## Skip the talk, I want to run some WALI apps!

1. Clone this repo
```shell
git clone https://github.com/arjunr2/WALI.git
```

2. Install dependencies
* Ninja
* Make
* Cmake
* GCC
* lld
* [WABT](https://github.com/WebAssembly/wabt)

If using `apt`, run the `./apt-install-deps.sh` script with privileges to install above depedencies

3. Build a WALI runtime
```shell
./mvp-setup.sh
```
An `iwasm` symlink executable should be generated in the root directory

4. The [wasm-apps](wasm-apps) directory has several popular applications like Bash, Lua, and Sqlite
with sample scripts/data for each app.
As an example, to run `sqlite3`:
```shell
# Increase the stack size if the program runs out of space
./iwasm -v=0 --stack-size=262144 wasm-apps/sqlite/sqlite3.wasm
```


## Building the Entire Toolchain

Before proceeding, make sure all dependencies are up to date, as detailed in previous [section](#skip-the-talk-i-want-to-run-some-wali-apps):

There are four major toolchain components: 
* Custom Clang compiler (C -> Wasm-WALI)
* C-standard library for WALI
* WALI runtime
* (Optional) AoT Compiler for WAMR (Wasm-WALI -> WAMR AoT)

If compilation capability is not required, only the WALI runtime needs to be built. 
In this case, skip to [building the runtime](#building-wali-runtime)


### Building the Wasm-WALI Clang compiler

We use LLVM Clang 16. Requires *compiler-rt* builtins from LLVM 16 for wasm32 for full libc support, which is
included in the [misc](misc) directory.
To build the llvm suite:

```shell
git submodule update --init llvm-project
make wali-compiler
```

Add the llvm build binary directory (`<root-directory>/llvm-project/build/bin`) to PATH for convenience as the default compiler.


### Building WALI libc

The [wali-musl](https://github.com/arjunr2/wali-musl) submodule has detailed information on prerequisites and 
steps for compiling libc

Once the clang compiler is installed, you may pull+build libc as such:
```shell
git submodule update --init wali-musl
make libc
```

We currently support 64-bit architectures for x86-64, aarch64, and riscv64. In the future, we will expand
to more architectures.


### Building WALI runtime

We produce a baseline implementation in [WAMR](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali).
For details on how to implement these native APIs in WAMR, refer [here](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md)

The following steps will pull+build the WALI implementation of the runtime
```shell
git submodule update --init wasm-micro-runtime
make iwasm
```

### (Optional) WAMR AoT Compiler

Refer [here](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/a29e5c633c26a30e54373f658394fab2b95f394e/wamr-compiler)
on steps to build the AoT compiler.

Once completed, you can create a symlink from the root directory:
```shell
ln -sf wasm-micro-runtime/wamr-compiler/wamrc wamrc
```


## Compiling Applications to WALI

### Standalones

To compile C to WASM, refer to
[compile-wali-standalone.sh](tests/compile-wali-standalone.sh):

```shell
# Compile standalone C file
clang \
  --target=wasm32-wasi-threads -O3 -pthread \
  `# Sysroot and lib search path` \
  --sysroot=<path-to-wali-sysroot> -L<path-to-wali-sysroot>/lib \
  `# Enable wasm extension features`  \
  -matomics -mbulk-memory -mmutable-globals -msign-ext  \
  `# Linker flags for shared mem + threading` \
  -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=67108864 \
  <input-c-file> -o <output-wasm-file>
```

Since changes are yet to be made to `clang/wasm-ld` for the wali toolchain, we are using support enabled 
in `wasi-threads` target. This will change once a `wasm32-linux` target is added for WALI.

To indepedently specify compile and link flags, refer to [compile-wali.sh](tests/compile-wali.sh) in the test suite compilation toolchain

### Building the Test Suite
```shell
make tests
```

The above target builds all the C files in [tests](tests) using the above script and generates the WASM executables `tests/wasm/*_link.wasm`, which
can be executed by a WALI-supported runtime. It also generates native ELF files for the respective tests in `tests/elf` to compare
against the WASM output


### WASM Bytecode -> AoT Compilation

Use the WAMR compiler `wamrc` as detailed in their README. Include the 
`--enable-multi-thread` flag to generate threaded code



## Running WALI-WASM code

Use any Webassembly runtime that implements WALI to execute the above generated WASM code.

If you built the baseline WAMR implementation from the Makefile,
you can use `./iwasm <path-to-wasm-file>` to execute the code.

The [wasm-apps](wasm-apps) directory has several popular prebuilt binaries to run. You may also
run the test suite binaries detailed [here](#building-the-test-suite)



## Miscellaneous

### Run WASM code like an ELF binary!

Most Linux distros will allow registration of miscellaneous binary formats.
For WASM binaries, the OS must be aware of which program to invoke to run the WASM file. 

1. Create a wrapper bash script around the runtime invocation as below
```shell
#!/bin/bash
# /usr/bin/iwasm-wrapper - Wrapper for running WASM programs

exec <full-path-to-iwasm> -v=0 --stack-size=262144 "$@"
```
2. Register WASM as a misc format and use the script from step 1 as the interpreter
```shell
cd misc
sudo ./binfmt_register.sh
```

**NOTE**: The above solution gets erased after reboots. For a more permanent setup using binfmt daemon:
```shell
sudo cp misc/iwali.conf /etc/binfmt.d/
sudo systemctl restart systemd-binfmt
```

More information about miscellaneous binary formats and troubleshooting can be found [here](https://docs.kernel.org/admin-guide/binfmt-misc.html)

## Resources
[Syscall Information Table](https://docs.google.com/spreadsheets/d/1__2NqMqGLHdjFFYonkF49IkGgfv62TJCpZuXqhXwnlc/edit?usp=sharing)

This paper (https://cseweb.ucsd.edu/~dstefan/pubs/johnson:2022:wave.pdf) and its related work section, especially the bit labeled "Modeling and verifying system interfaces"

