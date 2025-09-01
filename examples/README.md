# WALI Examples

To compile a simple applications, you can run:
```shell
# Includes all the necessary flags to compile 
./compile-wali-standalone.sh -o printf.wasm printf.c
```
A number of precompiled, larger applications are available under `precompiled`

Note: If these applications use dynamic libraries/linking, these are disabled due to lack of support in WASM

