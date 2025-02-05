## Constraints in WALI applications during runtime

Wasm enforces type-checking unlike C, so **type-unsafe C code may not produce the desired output at runtime**.
In particular, we have observed the following common unsafe occurences:
* **Indirect function invocation with function pointers**: Unlike C, Wasm's `call_indirect` performs a runtime type-check which will fail on function invocation with mismatched signatures.
* **Variadic function types**: Functions that use variadic arguments *must* ensure argument type consistency (e.g. `syscall` arguments must all be typecast to `long`).

Additonally, there are certain WALI-inherent extra restrictions for certain applications:
* **Restricted Signals**: Some signals (e.g. SIGSEGV) are used by the engine, and apps that overwrite those signal handlers will not operate at intended.
* **Certain Filesystem endpoints**: e.g. `/proc/mem`
