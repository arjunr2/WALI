set (CMAKE_SYSTEM_NAME Linux)

set (WALI_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}) 
set (WALI_LLVM_BIN_DIR ${WALI_ROOT_DIR}/llvm-project/build/bin)

set (WALI_WASM_FEATURE_FLAGS "-matomics -mbulk-memory -mmutable-globals -msign-ext -mexception-handling")
set (WALI_LIBCXX_DIR ${WALI_ROOT_DIR}/libcxx)

# Sysroot
set (CMAKE_SYSROOT ${WALI_ROOT_DIR}/wali-musl/sysroot)

# Compiler toolchain
set (CMAKE_C_COMPILER ${WALI_LLVM_BIN_DIR}/clang)
set (CMAKE_CXX_COMPILER ${WALI_LLVM_BIN_DIR}/clang++)
set (CMAKE_AR ${WALI_LLVM_BIN_DIR}/llvm-ar)
set (CMAKE_RANLIB ${WALI_LLVM_BIN_DIR}/llvm-ranlib)
set (CMAKE_LINKER ${WALI_LLVM_BIN_DIR}/wasm-ld)

# Flags
set (CMAKE_C_FLAGS "--target=wasm32-wasi-threads -pthread --sysroot=${CMAKE_SYSROOT} ${WALI_WASM_FEATURE_FLAGS}")
set (CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_C_FLAGS} -I${WALI_ROOT_DIR}/libcxx/include/c++/v1")
set (CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_SYSROOT}/lib -L${WALI_LIBCXX_DIR}/lib -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=2147483648)
