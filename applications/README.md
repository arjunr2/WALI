# WALI-compiled Applications

## Prerequisites

Some builds (e.g. bash) generate and execute intermediate WALI binaries in their
build process. 
Ensure the [miscellaneous binary format](https://github.com/arjunr2/WALI/tree/main?tab=readme-ov-file#miscellaneous) registration for seamless builds.

## Building Applications

This folder contains all applications and libraries ported to WALI. To build applications:
```shell
git submodule update --init .
make <app/lib name>
# To clean, run make <app/lib name>-clean
```


## Testing LIBUVWASI
```shell
./run_libuvwasi_tests.sh  # Compiles and runs the entire uvwasi ctest suite
```

Add the option `--build` if you want to re-build the UVWASI implementation beforehand
