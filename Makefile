
IWASM_DIR=wasm-micro-runtime/product-mini/platforms/linux/build
#TEST_DIR_ARGS=WASM_DIR=../wasm ELF_DIR=../elf

.PHONY: default libc iwasm wali-compiler tests clean clean-iwasm

default: iwasm

all: libc iwasm wali-compiler tests

iwasm-dir:
	mkdir -p $(IWASM_DIR)

libc:
	make -C wali-musl

.ONESHELL:
iwasm: iwasm-dir
	cd $(IWASM_DIR)
	cmake -GNinja .. -DWAMR_BUILD_PLATFORM=linux -DWAMR_BUILD_CUSTOM_NAME_SECTION=1 \
		-DWAMR_BUILD_DUMP_CALL_STACK=1 -DWAMR_BUILD_LIB_WASI_THREADS=1 \
		-DWAMR_BUILD_MEMORY_PROFILING=1 -DWAMR_BUILD_LIBC_WALI=1
	ninja
	cd -
	ln -fs $(IWASM_DIR)/iwasm iwasm

.ONESHELL:
wali-compiler:
	cd llvm-project
	cmake -S llvm -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang;lldb" \
		-DLLVM_ENABLE_RUNTIMES=compiler-rt -DLLVM_PARALLEL_COMPILE_JOBS=8 -DLLVM_USE_LINKER=lld \
		-DLLVM_PARALLEL_LINK_JOBS=2
	cd build
	ninja
	mkdir -p lib/clang/16/lib/wasi
	cp ../../misc/libclang_rt.builtins-wasm32.a lib/clang/16/lib/
	cp ../../misc/libclang_rt.builtins-wasm32.a lib/clang/16/lib/wasi/

.ONESHELL:
wamr-compiler:
	cd wasm-micro-runtime/wamr-compiler
	./build_llvm.sh
	mkdir -p build && cd build
	cmake .. -GNinja
	ninja


tests: libc
	make -C tests -j8 $(TEST_DIR_ARGS)

clean-iwasm:
	rm -r $(IWASM_DIR)

clean: clean-iwasm
	make -C tests clean $(TEST_DIR_ARGS)
	make -C wali-musl clean
