.PHONY: all clean vim sqlite openssh mqtt-app lua
.PHONY: libs zlib libevent paho.mqtt.c ncurses uvwasi 

CURDIR = $(shell pwd)
MAKE = make

# ---- Sysroot ---- #
WALI_SYSROOT_DIR = $(CURDIR)/../wali-musl/sysroot
# ---- Compiler and Linker ---- #
WALI_LLVM_BIN_DIR = $(CURDIR)/../llvm-project/build/bin
WALI_CC = $(WALI_LLVM_BIN_DIR)/clang
WALI_LD = $(WALI_LLVM_BIN_DIR)/wasm-ld
WALI_AR = $(WALI_LLVM_BIN_DIR)/llvm-ar
WALI_RANLIB = $(WALI_LLVM_BIN_DIR)/llvm-ranlib


WALI_WARNING_CFLAGS = -Wno-implicit-function-declaration -Wno-int-conversion -Wno-incompatible-function-pointer-types
WALI_WASM_FEATURE_FLAGS = -matomics -mbulk-memory -mmutable-globals -msign-ext
# ---- Common flags ---- #
WALI_COMMON_CFLAGS = -O0 --target=wasm32-wasi-threads $(WALI_WARNING_CFLAGS) --sysroot=$(WALI_SYSROOT_DIR) -L$(WALI_SYSROOT_DIR)/lib $(WALI_WASM_FEATURE_FLAGS)
WALI_COMMON_LDFLAGS = -Wl,--shared-memory -Wl,--export-memory -Wl,--max-memory=67108864

# --- Library path --- #
WALI_LIB_SRC = $(CURDIR)/libs


all: vim sqlite openssh

libs: zlib libevent ncurses paho.mqtt.c uvwasi

.ONESHELL:

## --- Libraries --- ##
zlib:
	cd $(WALI_LIB_SRC)/zlib
	CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" LIBS="-lpthread"  AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes ./configure 
	$(MAKE)
zlib-clean:
	$(MAKE) -C $(WALI_LIB_SRC)/zlib clean

libevent:
	cd $(WALI_LIB_SRC)/libevent;
	mkdir -p build; cd build
	cmake .. -DCMAKE_C_COMPILER=$(WALI_CC) -DCMAKE_C_FLAGS="-pthread $(WALI_COMMON_CFLAGS)" -DCMAKE_LINKER=$(WALI_LD) -DCMAKE_LD_FLAGS="$(WALI_COMMON_LDFLAGS)" -DCMAKE_AR=$(WALI_AR) -DCMAKE_RANLIB=$(WALI_RANLIB) -DEVENT__DISABLE_OPENSSL=ON -DEVENT__LIBRARY_TYPE=STATIC
	$(MAKE)

libevent-clean:
	rm -rf $(WALI_LIB_SRC)/libevent/build

ncurses:
	cd $(WALI_LIB_SRC)/ncurses;
	mkdir -p build; cd build;
	../configure --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS=$(WALI_COMMON_LD_FLAGS) AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
ncurses-clean:
	rm -rf $(WALI_LIB_SRC)/ncurses/build

paho.mqtt.c:
	cd $(WALI_LIB_SRC)/paho.mqtt.c;
	mkdir -p build; cd build
	cmake .. -DCMAKE_C_COMPILER=$(WALI_CC) -DCMAKE_C_FLAGS="-pthread $(WALI_COMMON_CFLAGS)" -DCMAKE_LINKER=$(WALI_LD) -DCMAKE_LD_FLAGS="$(WALI_COMMON_LDFLAGS)" -DCMAKE_AR=$(WALI_AR) -DCMAKE_RANLIB=$(WALI_RANLIB) -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE
	$(MAKE)
paho.mqtt.c-clean:
	rm -rf $(WALI_LIB_SRC)/paho.mqtt.c/build

uvwasi:
	cd $(WALI_LIB_SRC)/uvwasi;
	mkdir -p out/cmake; cd out/cmake;
	cmake ../.. -DUVWASI_BUILD_SHARED=FALSE -DBUILD_TESTING=ON -DCMAKE_C_COMPILER=$(WALI_CC) -DCMAKE_C_FLAGS="-pthread $(WALI_COMMON_CFLAGS) $(WALI_COMMON_LDFLAGS)"
	$(MAKE)
uvwasi-clean:
	rm -rf $(WALI_LIB_SRC)/uvwasi/out

## --- Applications --- ##
vim: ncurses
	cd vim;
	$(MAKE) CC=$(WALI_CC) CClink="$(WALI_CC) $(WALI_COMMON_CFLAGS)" CFLAGS="$(WALI_COMMON_CFLAGS)" LDFLAGS="-L$(WALI_LIB_SRC)/ncurses/build/lib $(WALI_COMMON_LDFLAGS)" LD=$(WALI_LD) CONF_OPT_TERMINAL=--disable-terminal
vim-clean: ncurses-clean
	$(MAKE) -C vim clean

sqlite:
	mkdir -p sqlite/build; cd sqlite/build;
	../configure CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) BCC=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS) -U__wasi__" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
sqlite-clean:
	rm -rf sqlite/build

openssh: zlib
	mkdir -p openssh/build; cd openssh/build;
	../configure --without-openssl --with-zlib=$(WALI_LIB_SRC)/zlib CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS) $(WALI_COMMON_CFLAGS)" LIBS="-lpthread" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
openssh-clean: zlib-clean
	rm -rf openssh/build

mqtt-app: paho.mqtt.c
	$(WALI_CC) $(WALI_COMMON_CFLAGS) -L$(WALI_LIB_SRC)/paho.mqtt.c/build/src -I$(WALI_LIB_SRC)/paho.mqtt.c/src $(WALI_COMMON_LDFLAGS) -lm -lpaho-mqtt3c $@/mqtt.c -o $@/mqtt-app 
mqtt-app-clean: paho.mqtt.c-clean
	rm mqtt-app/mqtt-app

lua:
	cd lua;
	$(MAKE) CC=$(WALI_CC) MYCFLAGS="$(WALI_COMMON_CFLAGS) -std=c99 -DLUA_USE_LONGJMP -fno-stack-protector -fno-common" MYLDFLAGS="$(WALI_COMMON_CFLAGS) $(WALI_COMMON_LDFLAGS) -Wl,-E" MYLIBS="" AR="$(WALI_AR) rc" RANLIB=$(WALI_RANLIB)
lua-clean:
	make -C lua clean
	

clean: sqlite-clean openssh-clean vim-clean mqtt-app-clean uvwasi-clean lua-clean


