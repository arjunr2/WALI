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

## Skip the talk, I want to run some WALI apps!

1. Install dependencies
* Ninja
* Make
* Cmake
* GCC
* [WABT](https://github.com/WebAssembly/wabt)

If using `apt`, run `sudo ./apt-install-deps.sh` to install above depedencies

2. Build a WALI runtime following these [instructions](#building-wali-runtime)

3. The [wasm-apps](wasm-apps) directory has several popular applications like Bash, Lua, and Sqlite
with sample scripts/data for each app.
As an example, to run `sqlite3`:
```shell
# Increase the stack size if the program runs out of space
./iwasm -v=0 --stack-size=524288 wasm-apps/sqlite/sqlite3.wasm
```


## Building the Entire Toolchain

Before proceeding, make sure all dependencies are up to date, as detailed in previous [section](#skip-the-talk-i-want-to-run-some-wali-apps):

There are four major toolchain components: 
1. WALI runtime
2. Custom Clang compiler (C -> Wasm-WALI)
3. C-standard library for WALI
4. (Optional) AoT Compiler for WAMR (Wasm-WALI -> WAMR AoT)

If compiling WALI applications is not required and step 1 is required.

### Building WALI runtime

We produce a baseline implementation in [WAMR](https://github.com/SilverLineFramework/wasm-micro-runtime/tree/wali).
For details on how to implement these native APIs in WAMR, refer [here](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md)

To build the WAMR-WALI runtime:
```shell
git submodule update --init wasm-micro-runtime
make iwasm
```
An `iwasm` symlink executable should be generated in the root directory


### Building the Wasm-WALI Clang compiler

We use LLVM Clang 16 with *compiler-rt* builtins for full wasm32 support.
To build the llvm suite:

```shell
git submodule update --init llvm-project
make wali-compiler
```

Future steps use this toolchain.
Add the llvm build binary directory (`<root-directory>/llvm-project/build/bin`) to PATH for convenience.


### Building WALI libc

The [wali-musl](https://github.com/arjunr2/wali-musl) submodule has detailed information on prerequisites and 
steps for compiling libc

To build libc:
```shell
git submodule update --init wali-musl
make libc
```

We currently support 64-bit architectures for x86-64, aarch64, and riscv64. In the future, we will expand
to more architectures.


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
<path-to-WALI-clang> \
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

To indepedently specify compile and link flags, refer to [compile-wali.sh](tests/compile-wali.sh) used for the test suite

### Building the Test Suite
```shell
make tests
```

WALI executables are located in `tests/wasm`. 
Native ELF files for the same in `tests/elf` can be used to compare against the WASM output


### WASM Bytecode -> AoT Compilation

Use the WAMR compiler `wamrc` with the `--enable-multi-thread` flag to generate threaded code


## Running WALI-WASM code

Use any Webassembly runtime that implements WALI to execute the above generated WASM code.

If you built the baseline WAMR implementation from the Makefile,
you can use `./iwasm <path-to-wasm-file>` to execute the code.

The [wasm-apps](wasm-apps) directory has several popular prebuilt binaries to run. You may also
run the test suite binaries detailed [here](#building-the-test-suite)



## Miscellaneous

### Run WASM code like an ELF binary!

Most Linux distros will allow registration of miscellaneous binary formats.
This will **greatly** simplify all toolchain builds for WALI out-of-the-box and is highly recommended.
This is **required** to compile some [applications](applications) in our repo
To enable this, run the following:
```shell
cd misc
source misc/gen_iwasm_wrapper.sh
# Default binfmt_register does not survive reboots in the system
# Specify '-p' option to register with systemd-binfmt for reboot survival
sudo ./misc/binfmt_register.sh
```

This essentially points Linux to our `iwasm` interpreter to invoke any WASM/AoT file. 
More information about miscellaneous binary formats and troubleshooting can be found [here](https://docs.kernel.org/admin-guide/binfmt-misc.html)

## Resources
[Syscall Information Table](https://docs.google.com/spreadsheets/d/1__2NqMqGLHdjFFYonkF49IkGgfv62TJCpZuXqhXwnlc/edit?usp=sharing)

This paper (https://cseweb.ucsd.edu/~dstefan/pubs/johnson:2022:wave.pdf) and its related work section, especially the bit labeled "Modeling and verifying system interfaces"

