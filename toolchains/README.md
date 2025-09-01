# Toolchains

These files allow integration of WALI toolchains into common build systems and environments, including all necessary compiler artifacts and flags.

## Configuration

[wali.toml](wali.toml) specifies the high-level configuration. All toolchain files can thereafter we generated as follows:
```shell
python3 gen_toolchains.py
``` 

### Bash (wali.sh)

```shell
# Example usage of the bash toolchain in a shell:
# Sets environment variables (e.g. WALI_CC, WALI_LD, ...)
source wali.sh
$WALI_CC $WALI_COMMON_CFLAGS $WALI_COMMON_LDFLAGS <c-file> -o <output-file>
```

### Makefile (wali.mk)

```makefile
# Example usage in a Makefile:
include wali.mk

# Example usage
%.wasm: %.c
    $(WALI_CC) $(WALI_COMMON_CFLAGS) $(WALI_COMMON_LDFLAGS) -o %@ $<
```

### CMake (wali.cmake) 

```shell
# Use as toolchain file in cmake command
cmake -DCMAKE_TOOLCHAIN_FILE=wali.cmake ...
```