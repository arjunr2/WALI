## WALI Constraints

Wasm possesses static properties (e.g. stricter type-checking rules) and dynamic properties (e.g. sandboxing) than some low-level languages like C, so **type-unsafe C code may produce divergence from native binaries at runtime**.
In particular, we have observed the following occurences, that are usually the result of programs bending certain safety rules:
* **Indirect function invocation with function pointers**: Unlike C, Wasm's `call_indirect` performs a runtime type-check which will fail on function invocation with mismatched signatures.
* **Variadic function types**: Functions that use variadic arguments *must* ensure argument type consistency (e.g. `syscall` arguments must all be typecast to `long`).

Additonally, there are certain WALI-inherent extra restrictions for certain applications:
* **Restricted Signals**: Some signals (e.g. SIGSEGV) are used by the engine, and apps that overwrite those signal handlers will not operate at intended.
* **Certain Filesystem endpoints**: e.g. `/proc/mem` cannot be accessed through WALI.