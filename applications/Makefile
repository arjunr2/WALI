include ../wali_config.mk

MAKE = make

# --- Library path --- #
WALI_LIB_SRC = $(shell pwd)/libs

.PHONY: all clean vim sqlite openssh mqtt-app lua make memcached bash
.PHONY: libs zlib libevent paho.mqtt.c ncurses uvwasi 
.PHONY: make-clean zlib-clean

all: vim sqlite openssh mqtt-app lua make memcached bash

libs: zlib libevent ncurses paho.mqtt.c uvwasi

.ONESHELL:

## --- Libraries --- ##
zlib:
	cd $(WALI_LIB_SRC)/zlib
	CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" LIBS="-lpthread"  AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes ./configure 
	$(MAKE)
zlib-clean:
	cd $(WALI_LIB_SRC)/zlib
	$(MAKE) clean

libevent:
	cd $(WALI_LIB_SRC)/libevent;
	cmake -DCMAKE_C_COMPILER=$(WALI_CC) -DCMAKE_C_FLAGS="$(WALI_COMMON_CFLAGS) -L$(WALI_SYSROOT_DIR)/lib" -DCMAKE_LINKER=$(WALI_LD) -DCMAKE_LD_FLAGS="$(WALI_COMMON_LDFLAGS)" -DCMAKE_AR=$(WALI_AR) -DCMAKE_RANLIB=$(WALI_RANLIB) -DEVENT__DISABLE_OPENSSL=ON -DEVENT__LIBRARY_TYPE=STATIC
	$(MAKE)

libevent-clean:
	rm -rf $(WALI_LIB_SRC)/libevent/build

ncurses:
	cd $(WALI_LIB_SRC)/ncurses;
	mkdir -p build; cd build;
	../configure --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
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
	$(MAKE) CC="$(WALI_CC) --target=x86_64" CClink="$(WALI_CC) $(WALI_COMMON_CFLAGS)" CFLAGS="$(WALI_COMMON_CFLAGS)" LDFLAGS="-L$(WALI_LIB_SRC)/ncurses/build/lib $(WALI_COMMON_LDFLAGS)" LD=$(WALI_LD) CONF_OPT_TERMINAL=--disable-terminal
vim-clean: ncurses-clean
	cd vim;
	$(MAKE) clean

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
	rm -f mqtt-app/mqtt-app

lua:
	cd lua;
	$(MAKE) CC=$(WALI_CC) CFLAGS="-Wall -O2 $(WALI_COMMON_CFLAGS) -std=c99 -DLUA_USE_LONGJMP -fno-stack-protector -fno-common" MYLDFLAGS="$(WALI_COMMON_CFLAGS) $(WALI_COMMON_LDFLAGS) -Wl,-E" MYLIBS="" AR="$(WALI_AR) rc" RANLIB=$(WALI_RANLIB)
lua-clean:
	cd lua;
	$(MAKE) clean

make:
	cd make;
	autoreconf -f -i
	mkdir -p build; cd build
	../configure CC=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
make-clean:
	cd make/build;
	$(MAKE) clean

memcached: libevent
	cd memcached; ./autogen.sh
	mkdir -p build; cd build
	../configure --with-libevent="$(WALI_LIB_SRC)/libevent" CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="-pthread $(WALI_COMMON_CFLAGS)" LIBS="-lpthread" LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS) -lpthread" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
memcached-clean:
	rm -rf memcached/build
	
bash:
	cd bash;
	mkdir -p build; cd build
	../configure --disable-threads --without-bash-malloc  CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS) -DNO_MAIN_ENV_ARG" CFLAGS_FOR_BUILD="$(WALI_COMMON_CFLAGS) -DNO_MAIN_ENV_ARG" CPPFLAGS="$(WALI_COMMON_CFLAGS)" CPPFLAGS_FOR_BUILD="$(WALI_COMMON_CFLAGS)" LD=$(WALI_LD) LD_FOR_BUILD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" LDFLAGS_FOR_BUILD="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) SIZE="wasm-objdump -h" cross_compiling=yes CONFIG_SHELL=/bin/bash MAKE_SHELL=/bin/bash SHELL=/bin/bash
	$(MAKE)
bash-clean:
	rm -rf bash/build

clean: sqlite-clean openssh-clean vim-clean mqtt-app-clean uvwasi-clean lua-clean memcached-clean bash-clean


