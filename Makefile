include wali_config.mk

IWASM_BUILD_DIR := $(WALI_ROOT_DIR)/wasm-micro-runtime/product-mini/platforms/linux/build
LLVM_BUILD_DIR := $(WALI_LLVM_DIR)/build
MUSL_BUILD_DIR := $(WALI_ROOT_DIR)/wali-musl
LIBCXX_BUILD_DIR := $(WALI_ROOT_DIR)/libcxx

MEMGB := $(shell echo $$(free -g | sed -n '2p' | awk '{print $$2}'))

COMPILE_PARALLEL := $(shell nproc --all)
LINK_PARALLEL := $(shell echo $$(($(MEMGB) < 16 ? 1 : $(MEMGB) / 16)))

.PHONY: default libc libcxx iwasm wali-compiler llvm-base tests clean clean-iwasm clean-all clean-llvm
.PHONY: rustc

default: iwasm

all: libc iwasm wali-compiler libcxx tests

# --- STANDARD LIBRARIES --- #
libc:
	make -C $(MUSL_BUILD_DIR) -j$(COMPILE_PARALLEL)

# NOTE: Catching Exceptions only seems to work when libcxx is compiled in Debug mode (O0) #
.ONESHELL:
libcxx: wali-compiler libc
	cmake -S $(WALI_LLVM_DIR)/runtimes -B $(LIBCXX_BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=Debug  \
		-DCMAKE_C_COMPILER_WORKS=ON \
		-DCMAKE_CXX_COMPILER_WORKS=ON \
		-DCMAKE_POSITION_INDEPENDENT_CODE=OFF \
		-DLLVM_PARALLEL_COMPILE_JOBS=$(COMPILE_PARALLEL) -DLLVM_PARALLEL_LINK_JOBS=$(LINK_PARALLEL) \
		-DLLVM_CONFIG_PATH=$(WALI_LLVM_BIN_DIR)/llvm-config \
		-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
		-DCXX_SUPPORTS_CXX11=ON \
		-DLIBCXX_ENABLE_THREADS:BOOL=ON \
		-DLIBCXX_HAS_EXTERNAL_THREAD_API:BOOL=OFF \
		-DLIBCXX_BUILD_EXTERNAL_THREAD_LIBRARY:BOOL=OFF \
		-DLIBCXX_HAS_WIN32_THREAD_API:BOOL=OFF \
		-DLLVM_COMPILER_CHECKED=ON \
		-DLIBCXX_ENABLE_SHARED:BOOL=OFF \
		-DLIBCXX_ENABLE_EXPERIMENTAL_LIBRARY:BOOL=OFF \
		-DLIBCXX_ENABLE_EXCEPTIONS:BOOL=ON \
		-DLIBCXX_ENABLE_FILESYSTEM:BOOL=ON \
		-DLIBCXX_ENABLE_ABI_LINKER_SCRIPT:BOOL=OFF \
		-DLIBCXX_CXX_ABI=libcxxabi \
		-DLIBCXX_CXX_ABI_INCLUDE_PATHS=$(WALI_LLVM_BIN_DIR)/../../libcxxabi/include \
		-DLIBCXX_HAS_MUSL_LIBC:BOOL=ON \
		-DLIBCXX_ABI_VERSION=2 \
		-DLIBCXXABI_ENABLE_EXCEPTIONS:BOOL=ON \
		-DLIBCXXABI_ENABLE_SHARED:BOOL=OFF \
		-DLIBCXXABI_SILENT_TERMINATE:BOOL=ON \
		-DLIBCXXABI_ENABLE_THREADS:BOOL=ON \
		-DLIBCXXABI_HAS_PTHREAD_API:BOOL=ON \
		-DLIBCXXABI_HAS_EXTERNAL_THREAD_API:BOOL=OFF \
		-DLIBCXXABI_BUILD_EXTERNAL_THREAD_LIBRARY:BOOL=OFF \
		-DLIBCXXABI_HAS_WIN32_THREAD_API:BOOL=OFF \
		-DLIBCXXABI_ENABLE_PIC:BOOL=OFF \
		-DLIBCXXABI_USE_LLVM_UNWINDER:BOOL=ON \
		-DLIBCXXABI_ENABLE_STATIC_UNWINDER:BOOL=ON \
		-DLIBUNWIND_ENABLE_SHARED:BOOL=OFF \
		-DLIBUNWIND_ENABLE_THREADS:BOOL=ON \
		-DLIBUNWIND_ENABLE_CROSS_UNWINDING:BOOL=ON \
		-DLIBUNWIND_HIDE_SYMBOLS:BOOL=ON \
		-DUNIX:BOOL=ON	\
		--debug-trycompile \
		-DCMAKE_C_COMPILER=$(WALI_CC) -DCMAKE_CXX_COMPILER=$(WALI_CXX) -DCMAKE_LINKER=$(WALI_LD) -DCMAKE_AR=$(WALI_AR) -DCMAKE_RANLIB=$(WALI_RANLIB) \
		-DCMAKE_C_FLAGS="-pthread $(WALI_COMMON_CFLAGS)" \
		-DCMAKE_ASM_FLAGS="$(WALI_COMMON_CFLAGS)" \
		-DCMAKE_CXX_FLAGS="-pthread -Wno-user-defined-literals $(WALI_COMMON_CFLAGS)" \
		-DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind"
	cd $(LIBCXX_BUILD_DIR)
	make -j$(COMPILE_PARALLEL)
	cp libcxx/lib/* $(WALI_SYSROOT_DIR)/lib/
	cp libcxx/include/* $(WALI_SYSROOT_DIR)/include/


# --- WAMR RUNTIME/COMPILER --- #
iwasm-dir:
	mkdir -p $(IWASM_BUILD_DIR)

.ONESHELL:
iwasm: iwasm-dir
	cd $(IWASM_BUILD_DIR)
	cmake -GNinja .. -DCMAKE_BUILD_TYPE=Release -DWAMR_BUILD_PLATFORM=linux -DWAMR_BUILD_CUSTOM_NAME_SECTION=1 \
		-DWAMR_BUILD_DUMP_CALL_STACK=1 -DWAMR_BUILD_LIB_WASI_THREADS=1 -DWAMR_BUILD_EXCE_HANDLING=1 \
		-DWAMR_BUILD_MEMORY_PROFILING=0 -DWAMR_BUILD_LIBC_WALI=1 -DWAMR_BUILD_FAST_INTERP=0
	ninja
	cd -
	ln -fs $(IWASM_BUILD_DIR)/iwasm iwasm

.ONESHELL:
wamr-compiler:
	cd wasm-micro-runtime/wamr-compiler
	./build_llvm.sh
	mkdir -p build && cd build
	cmake -GNinja .. -DWAMR_BUILD_LIBC_WALI=1 -DWAMR_BUILD_GC=0
	ninja


# --- LLVM COMPILER --- #
.ONESHELL:
llvm-base:
	cmake -S $(WALI_LLVM_DIR)/llvm -B $(LLVM_BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DLLVM_ENABLE_PROJECTS="lld;clang;lldb" \
		-DLLVM_PARALLEL_COMPILE_JOBS=$(COMPILE_PARALLEL) -DLLVM_PARALLEL_LINK_JOBS=$(LINK_PARALLEL) \
		-DLLVM_USE_LINKER=lld \
		-DLLVM_STATIC_LINK_CXX_STDLIB=ON -DLLVM_STATIC_LINK_CXX_STDLIB=ON \
		-DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF \
		-DCMAKE_C_FLAGS="-fdebug-types-section" -DCMAKE_CXX_FLAGS="-fdebug-types-section"
	cd $(LLVM_BUILD_DIR)
	ninja

.ONESHELL:
wali-compiler: llvm-base
	LLVM_MAJOR_VERSION=`$(WALI_LLVM_BIN_DIR)/llvm-config --version | cut -d '.' -f 1`
	LIBCLANG_RT_LIB=$(WALI_ROOT_DIR)/misc/libclang_rt/llvm-$$LLVM_MAJOR_VERSION.libclang_rt.builtins-wasm32.a
	DEST_RT_DIR=$(WALI_LLVM_DIR)/build/lib/clang/$$LLVM_MAJOR_VERSION/lib
	mkdir -p $(WALI_LLVM_DIR)/build/lib/clang/$$LLVM_MAJOR_VERSION/lib/wasi
	cp $$LIBCLANG_RT_LIB $$DEST_RT_DIR/libclang_rt.builtins-wasm32.a
	cp $$LIBCLANG_RT_LIB $$DEST_RT_DIR/wasi/libclang_rt.builtins-wasm32.a
	

# --- COMPILER PORTS --- #
.ONESHELL:
rustc:
	export WASM_MUSL_SYSROOT=$(WALI_SYSROOT_DIR)
	cd compiler_ports/rust
	./x setup compiler
	python3 $(WALI_ROOT_DIR)/scripts/rustc_config.py -r $(WALI_ROOT_DIR)/compiler_ports/rust \
		-m $(WALI_SYSROOT_DIR) -l $(WALI_LLVM_BIN_DIR)
	cargo update -p libc
	cargo update --manifest-path src/bootstrap/Cargo.toml -p cc
	./x build
	rustup toolchain link wali build/host/stage1

# --- TESTS --- #
tests: libc
	make -C tests -j$(COMPILE_PARALLEL) $(TEST_DIR_ARGS)


# --- CLEANUP --- #
clean-iwasm:
	rm -rf $(IWASM_BUILD_DIR)

clean: clean-iwasm
	make -C tests clean $(TEST_DIR_ARGS)
	make -C $(MUSL_BUILD_DIR) clean
	rm -rf $(LIBCXX_BUILD_DIR)

clean-llvm:
	rm -rf $(WALI_LLVM_DIR)/build

clean-all: clean clean-llvm

