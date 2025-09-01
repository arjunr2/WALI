include ../toolchains/wali.mk

MAKE = make

# --- Library path --- #
WALI_LIB_SRC = $(shell pwd)/libs
LIB_PATCH_DIR = $(WALI_LIB_SRC)/patches
PATCH_COMMAND = git apply --reject

.PHONY: all clean vim sqlite openssh mqtt-app lua make memcached bash
.PHONY: libs zlib libevent paho.mqtt.c ncurses uvwasi libX11 libxtrans openssl
.PHONY: libxdmcp xorgproto xcbproto libxau libxcb x11test
.PHONY: make-clean zlib-clean

all: vim sqlite openssh mqtt-app lua make memcached bash

libs: zlib libevent ncurses paho.mqtt.c uvwasi libX11 openssl

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
	cd $(WALI_LIB_SRC)/libevent
	cmake -DCMAKE_VERBOSE_MAKEFILE=1 -DCMAKE_C_COMPILER=$(WALI_CC) -DCMAKE_C_FLAGS="$(WALI_COMMON_CFLAGS) $(WALI_COMMON_LDFLAGS)" -DCMAKE_LINKER=$(WALI_LD) -DCMAKE_LD_FLAGS="$(WALI_COMMON_LDFLAGS)" -DCMAKE_AR=$(WALI_AR) -DCMAKE_RANLIB=$(WALI_RANLIB) -DEVENT__DISABLE_OPENSSL=ON -DEVENT__LIBRARY_TYPE=STATIC
	$(MAKE)
libevent-clean:
	rm -rf $(WALI_LIB_SRC)/libevent/bin $(WALI_LIB_SRC)/libevent/lib

ncurses:
	cd $(WALI_LIB_SRC)/ncurses;
	mkdir -p build; cd build;
	../configure --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
ncurses-clean:
	rm -rf $(WALI_LIB_SRC)/ncurses/build

xorgproto:
	cd $(WALI_LIB_SRC)/xorgproto;
	mkdir -p build; cd build;
	mkdir -p install;
	../configure --prefix=$(WALI_LIB_SRC)/xorgproto/build/install --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE) install
xorgproto-clean:
	rm -rf $(WALI_LIB_SRC)/xorgproto/build

libxtrans:
	cd $(WALI_LIB_SRC)/libxtrans;
	mkdir -p build; cd build;
	mkdir -p install;
	../configure --prefix=$(WALI_LIB_SRC)/libxtrans/build/install --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
	$(MAKE) install
libxtrans-clean:
	rm -rf $(WALI_LIB_SRC)/libxtrans/build

xcbproto:
	cd $(WALI_LIB_SRC)/xcbproto;
	mkdir -p build; cd build;
	../configure --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
xcbproto-clean:
	rm -rf $(WALI_LIB_SRC)/xcbproto/build

libxau: xorgproto
	cd $(WALI_LIB_SRC)/libxau;
	mkdir -p build; cd build;
	mkdir -p install;
	../configure --prefix=$(WALI_LIB_SRC)/libxau/build/install --enable-static --disable-shared --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS) -I$(WALI_LIB_SRC)/xorgproto/build/install/include" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
	$(MAKE) install
libxau-clean:
	rm -rf $(WALI_LIB_SRC)/libxau/build

libxdmcp: xorgproto
	cd $(WALI_LIB_SRC)/libxdmcp;
	mkdir -p build; cd build;
	../configure --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS) -I$(WALI_LIB_SRC)/xorgproto/build/install/include" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
libxdmcp-clean:
	rm -rf $(WALI_LIB_SRC)/libxdmcp/build	

libxcb: libxau xorgproto
	cd $(WALI_LIB_SRC)/libxcb;
	mkdir -p build; cd build;
	mkdir -p install;
	../configure --prefix=$(WALI_LIB_SRC)/libxcb/build/install --enable-static --disable-shared --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS) -I$(WALI_LIB_SRC)/libxau/build/install/include -I$(WALI_LIB_SRC)/xorgproto/build/install/include" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE)
	$(MAKE) install
libxcb-clean:
	rm -rf $(WALI_LIB_SRC)/libxcb/build	

libX11: xorgproto libxcb libxtrans
	cd $(WALI_LIB_SRC)/libX11;
	mkdir -p build; cd build;
	mkdir -p install;
	../configure --prefix=$(WALI_LIB_SRC)/libX11/build/install --enable-malloc0returnsnull --enable-static --disable-shared --without-cxx --without-cxx-binding CC=$(WALI_CC) CC_FOR_BUILD=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS) -I$(WALI_LIB_SRC)/xorgproto/build/install/include -I$(WALI_LIB_SRC)/libxtrans/build/install/include -I$(WALI_LIB_SRC)/libxcb/build/install/include" BCC=$(WALI_CC) LD=$(WALI_LD) LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) cross_compiling=yes
	$(MAKE) -j$(nproc)
	$(MAKE) install
libX11-clean:
	rm -rf $(WALI_LIB_SRC)/libX11/build	

paho.mqtt.c:
	cd $(WALI_LIB_SRC)/paho.mqtt.c;
	mkdir -p build; cd build
	cmake .. -DCMAKE_C_COMPILER=$(WALI_CC) -DCMAKE_C_FLAGS="-pthread $(WALI_COMMON_CFLAGS) -L$(WALI_SYSROOT_DIR)/lib" -DCMAKE_LINKER=$(WALI_LD) -DCMAKE_LD_FLAGS="$(WALI_COMMON_LDFLAGS)" -DCMAKE_AR=$(WALI_AR) -DCMAKE_RANLIB=$(WALI_RANLIB) -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_ENABLE_TESTING=OFF
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

openssl:
	cd $(WALI_LIB_SRC)/openssl
	$(PATCH_COMMAND) $(LIB_PATCH_DIR)/openssl.patch
	mkdir -p build; cd build;
	CC=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS)" LDFLAGS="$(WALI_COMMON_LDFLAGS)" AR=$(WALI_AR) RANLIB=$(WALI_RANLIB) CXXFLAGS="$(WALI_COMMON_CFLAGS)" ../Configure --banner="wasm32-wali" -static no-asm no-async no-egd no-ktls no-module no-afalgeng no-secure-memory wasm32-wali
	$(MAKE) -j

openssl-clean:
	rm -rf $(WALI_LIB_SRC)/openssl/build

## --- Applications --- ##
vim: ncurses
	cd vim;
	$(MAKE) CC="$(WALI_CC)" CClink="$(WALI_CC) $(WALI_COMMON_CFLAGS)" CFLAGS="$(WALI_COMMON_CFLAGS) -Wno-incompatible-pointer-types" LDFLAGS="-L$(WALI_LIB_SRC)/ncurses/build/lib $(WALI_COMMON_LDFLAGS)" LD=$(WALI_LD) CONF_OPT_TERMINAL=--disable-terminal
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
	mkdir -p openssh/build
	cd openssh
	autoreconf -i
	cd build
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

x11test: libX11
	cd x11test;
	$(MAKE) CC=$(WALI_CC) CFLAGS="$(WALI_COMMON_CFLAGS) -I$(WALI_LIB_SRC)/libX11/build/install/include -I$(WALI_LIB_SRC)/xorgproto/build/install/include" LDFLAGS="-L$(WALI_LIB_SRC)/libX11/build/install/lib -L$(WALI_LIB_SRC)/libxcb/build/install/lib -L$(WALI_LIB_SRC)/libxau/build/install/lib $(WALI_COMMON_LDFLAGS)" LD=$(WALI_LD)
x11test-clean:
	cd x11test;
	$(MAKE) clean

clean: sqlite-clean openssh-clean vim-clean mqtt-app-clean uvwasi-clean lua-clean memcached-clean bash-clean xorgproto-clean xcbproto-clean libxau-clean libxtrans-clean libxdmcp-clean libxcb-clean libX11-clean x11test-clean


