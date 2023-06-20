.PHONY: build clean

SYSROOT_DIR=/home/arjun/Documents/research/webassembly/wali/wali-musl/sysroot

build:
	clang -v --target=wasm32-wasi-threads -O3 --sysroot=$(SYSROOT_DIR) -L$(SYSROOT_DIR)/lib \
    -L./lib -Iinclude -matomics -mbulk-memory -mmutable-globals -msign-ext \
    -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=67108864 \
    -lm -lpaho-mqtt3c -o benchmark mqtt.c

clean:
	rm -f benchmark
