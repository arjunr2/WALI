# WALI Test Harness

This directory contains a refactored test harness for WALI.

## Structure

- **`harness.py`**: The main driver. It compiles a C test file to both Native (ELF) and WASM, runs them, and compares the output written to a shared memory segment.
- **`wali_test.h`**: Header file defining the test interface (`wali_test_main`) and helper functions (`wali_test_log`, etc.) for writing to the shared memory.
- **`native_runner.c`**: The Native ELF runner. It sets up the shared memory, forks, and calls the test function in the child process.
- **`wasm_runner.c`**: The WASM runner. It sets up the shared memory (via WALI syscalls) and calls the test function.
- **`simple_test.c`**: A sample test case.

## Usage

To run a test:

```bash
python3 harness.py <test_file.c>
```

Example:

```bash
python3 harness.py simple_test.c
```

## Writing Tests

1. Create a C file (e.g., `my_test.c`).
2. Include `"wali_test.h"`.
3. Implement `int wali_test_main(int argc, char** argv)`.
4. Use `wali_test_log()` or `wali_test_write()` to record results.
5. **Do not** use `main()`.
6. Add configuration comments at the top:
   ```c
   /*
    * RUN: args="arg1 arg2" env="VAR=VAL"
    */
   ```

## How it works

- **Native**: The harness compiles `native_runner.c` + `my_test.c`. The runner `mmap`s a shared file, forks, and the child executes `wali_test_main`. The child inherits the memory mapping.
- **WASM**: The harness compiles `wasm_runner.c` + `my_test.c` to WASM. The runner `mmap`s the shared file (using WALI `mmap`) and executes `wali_test_main`.
- **Comparison**: The harness compares the binary content of the shared memory files from both runs.
