.PHONY: all clean libs vim sqlite openssh zlib libevent

CURDIR = $(shell pwd)

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
WALI_LIBS = $(CURDIR)/../libraries

WALI_LIB_SRC = $(CURDIR)/libs


all: vim sqlite openssh

libs: zlib libevent ncurses

.ONESHELL:

## --- Libraries --- ##
zlib:
	cd $(WALI_LIB_SRC)/zlib
	CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" LIBS="-lpthread"  AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes ./configure 
	make
zlib-clean:
	make -C $(WALI_LIB_SRC)/zlib clean

libevent:
	cd $(WALI_LIB_SRC)/libevent;
	mkdir -p build; cd build
	cmake .. -DCMAKE_C_COMPILER=$(WALI_CC) -DCMAKE_C_FLAGS="-pthread $(WALI_COMMON_CFLAGS)" -DCMAKE_LINKER=$(WALI_LD) -DCMAKE_LD_FLAGS="$(WALI_COMMON_LDFLAGS)" -DCMAKE_AR=$(WALI_AR) -DCMAKE_RANLIB=$(WALI_RANLIB) -DEVENT__DISABLE_OPENSSL=ON -DEVENT__LIBRARY_TYPE=STATIC
	make

libevent-clean:
	rm -rf $(WALI_LIB_SRC)/libevent/build

ncurses:
	cd $(WALI_LIB_SRC)/ncurses;
	mkdir -p build; cd build;
	../configure CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS=$(WALI_COMMON_LD_FLAGS) AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	make
ncurses-clean:
	rm -rf $(WALI_LIB_SRC)/ncurses/build


## --- Applications --- ##
vim:
	cd vim;
	make CC=$(WALI_CC) CClink="$(WALI_CC) $(WALI_COMMON_CFLAGS)" CFLAGS="$(WALI_COMMON_CFLAGS)" LDFLAGS="-L$(WALI_LIBS)/libncurses $(WALI_COMMON_LDFLAGS)" LD=$(WALI_LD) CONF_OPT_TERMINAL=--disable-terminal
vim-clean:
	make -C vim clean

sqlite:
	mkdir -p sqlite/build; cd sqlite/build;
	../configure CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) BCC=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS) -U__wasi__" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	make
sqlite-clean:
	rm -rf sqlite/build

openssh: zlib
	mkdir -p openssh/build; cd openssh/build;
	../configure --without-openssl --with-zlib=$(WALI_LIB_SRC)/zlib CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS) $(WALI_COMMON_CFLAGS)" LIBS="-lpthread" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	make
openssh-clean: zlib-clean
	rm -rf openssh/build


clean: sqlite-clean openssh-clean vim-clean ncurses-clean


