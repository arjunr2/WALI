
IWASM_DIR=wasm-micro-runtime/product-mini/platforms/linux/build
#TEST_DIR_ARGS=WASM_DIR=../wasm ELF_DIR=../elf

.PHONY: libc iwasm tests

all: libc iwasm tests

libc:
	make -C wali-musl

iwasm-dir:
	mkdir -p $(IWASM_DIR)

.ONESHELL:
iwasm: iwasm-dir
	cd $(IWASM_DIR)
	cmake -GNinja .. -DWAMR_BUILD_PLATFORM=linux -DWAMR_BUILD_CUSTOM_NAME_SECTION=1 -DWAMR_BUILD_MULTI_MODULE=1 \
		-DWAMR_BUILD_FAST_INTERP=1 -DWAMR_BUILD_LIB_PTHREAD=1 -DWAMR_BUILD_DUMP_CALL_STACK=1 -DWAMR_BUILD_LIB_WASI_THREADS=1
	ninja

tests: libc
	make -C tests -j8 $(TEST_DIR_ARGS)

clean:
	make -C tests clean $(TEST_DIR_ARGS)
	make -C wali-musl clean
	rm -r $(IWASM_DIR)
