# Webassembly Linux Interface (WALI)

This repo serves to document design decisions, requirements, and specifications for the WALI.

## Overview
We create a custom modified [musl libc](https://github.com/arjunr2/wali-musl) that uses WALI
and produce a baseline implementation in [WAMR](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali)

## Skip the talk, I want to run some things!

1. Install dependencies
```shell
sudo apt update
sudo apt install make cmake ninja-build gcc
```

2. Clone the repo
```shell
git clone https://github.com/arjunr2/WALI.git
git submodule init wasm-micro-runtime
git submodule update wasm-micro-runtime
```

2. Build the WALI webassembly runtime
```shell
make iwasm
```
The runtime executable is located in the path specified in the *IWASM_DIR* variable in the root Makefile

3. The [wasm-apps](wasm-apps) directory has several popular applications like Bash, Lua, and Sqlite.
As an example to run sqlite (a sample database and script are included in the directory):
```shell
cd wasm-apps/sqlite
# Increase the stack size if the program runs out of space
<path-to-iwasm> -v=0 --stack-size=262144 sqlite3.wasm
```


## Getting Started with the Toolchain

There are three major components for testing: the Wasm-WALI compiler, WALI libc, WALI runtime. 
Initial setup and packages may be required for some of the components detailed below.

`wasm-micro-runtime` submodule contains the required runtime. In case you don't need the 
entire compilation toolchain, skip to [building the runtime](#building-wali-runtime) after
installing the dependencies. `wali-musl` and 
`llvm-project` are only needed if you want the entire compilation toolchain.

### Install Dependencies

```shell
sudo apt update
sudo apt install make cmake ninja-build gcc
```

### Building the compiler

We use LLVM Clang 16. Requires *compiler-rt* builtins from LLVM 16 for wasm32 for full libc support.
To build the llvm suite:

```shell
cd llvm-project
cmake -S llvm -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang;lldb" \
  -DLLVM_ENABLE_RUNTIMES=compiler-rt -DLLVM_PARALLEL_COMPILE_JOBS=8 -DLLVM_USE_LINKER=lld \
  -DLLVM_PARALLEL_LINK_JOBS=2
cd build
ninja
```

Add the llvm toolchain build to PATH so the clang built is used as the default compiler. This is necessary for 
building the following components.



### Building WALI libc

The [wali-musl](wali-musl) submodule has detailed information on prerequisites and steps for compiling libc

Once the initial setup is performed for building libc, you may use the makefile target:
```shell
make libc
```

We currently support 64-bit architectures for x86-64, AARCH64, and RISCV64. In the future, we will expand
to more architectures.


### Building WALI runtime

We produce a baseline implementation in [WAMR](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali).
For details on how to implement these native APIs in WAMR, refer [here](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md)

After cloning the wasm-micro-runtime submodule, the following target will build the WALI implementation of the runtime
```shell
make iwasm
```
You may add the *IWASM_DIR* in the Makefile to your PATH to access the *iwasm* executable.


## Running WALI-WASM code

Use any Webassembly runtime that implements WALI to execute the above generated WASM code.

If you built the baseline WAMR implementation and added the *IWASM_DIR* from the Makefile to your path,
you can use `iwasm <path-to-wasm-file>` to execute the code.

The [wasm-apps](wasm-apps) directory has several popular prebuilt binaries to run. You may also
run the test suite binaries detailed [here](#building-the-test-suite)


## Compiling Applications to WALI

### Standalones

To compile C to WASM, refer to
[compile-wali-standalone.sh](tests/compile-wali-standalone.sh):

```shell
# Compile standalone C file
clang \
  --target=wasm32-wasi-threads -O3  \
  `# Sysroot and lib search path` \
  --sysroot=<path-to-wali-sysroot> -L<path-to-wali-sysroot>/lib \
  `# Enable wasm extension features`  \
  -matomics -mbulk-memory -mmutable-globals -msign-ext  \
  `# Linker flags for shared mem + threading` \
  -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=67108864 \
  <input-c-file> -o <output-wasm-file>
```

clang must use the one built in the earlier step. Since changes are yet to be made to `clang/wasm-ld` for the wali toolchain, we are using support enabled 
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



## Miscellaneous

### Register WASM as a miscellaneous binary format

Imagine running WASM code like any ELF binary on your machine! The OS must be aware of which program to invoke to
run the WASM file. 

1. Create a wrapper bash script around the runtime invocation as below
```shell
#!/bin/bash
# /usr/bin/iwasm-wrapper - Wrapper for running WASM programs

exec <full-path-to-iwasm> -v=0 --stack-size=262144 "$@"
```
2. Register WASM as a misc format and use the script from step 1 as the interpreter
```shell
sudo echo ':iwasm-interp:M::\x00asm::/usr/bin/iwasm-wrapper:' > /proc/sys/fs/binfmt_misc/register
```

More information about miscellaneous binary formats and troubleshooting can be found [here](https://docs.kernel.org/admin-guide/binfmt-misc.html)

## Resources
[Syscall Information Table](https://docs.google.com/spreadsheets/d/1__2NqMqGLHdjFFYonkF49IkGgfv62TJCpZuXqhXwnlc/edit?usp=sharing)

This paper (https://cseweb.ucsd.edu/~dstefan/pubs/johnson:2022:wave.pdf) and its related work section, especially the bit labeled "Modeling and verifying system interfaces"

