set (CMAKE_SYSTEM_NAME Linux)

set (ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}) 
set (WALI_LLVM_BIN_DIR ${ROOT_DIR}/llvm-project/build/bin)

# Sysroot
set (CMAKE_SYSROOT ${ROOT_DIR}/wali-musl/sysroot)

# Compiler toolchain
set (CMAKE_C_COMPILER ${WALI_LLVM_BIN_DIR}/clang)
set (CMAKE_CXX_COMPILER ${WALI_LLVM_BIN_DIR}/clang++)
set (CMAKE_AR ${WALI_LLVM_BIN_DIR}/llvm-ar)
set (CMAKE_RANLIB ${WALI_LLVM_BIN_DIR}/llvm-ranlib)
set (CMAKE_LINKER ${WALI_LLVM_BIN_DIR}/wasm-ld)

# Flags
set (CMAKE_C_FLAGS "--target=wasm32-wasi-threads -pthread --sysroot=${CMAKE_SYSROOT} -matomics -mbulk-memory -mmutable-globals -msign-ext")
set (CMAKE_CXX_FLAGS "-stdlib=libc++ --target=wasm32-wasi-threads -pthread --sysroot=${CMAKE_SYSROOT} -I${ROOT_DIR}/libcxx/include/c++/v1 -matomics -mbulk-memory -mmutable-globals -msign-ext")
set (CMAKE_EXE_LINKER_FLAGS "-L${CMAKE_SYSROOT}/lib -L${ROOT_DIR}/libcxx/lib -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=2147483648")
