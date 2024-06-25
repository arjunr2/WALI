include wazi_config.mk

wz_mkfile_path := $(abspath $(firstword $(MAKEFILE_LIST)))
WAZI_DIR := $(patsubst %/,%, $(dir $(wz_mkfile_path)))

HOOK_C := $(WAZI_DIR)/picolibc_zephyr_hooks.c
HOOK_O := $(HOOK_C:.c=.o)

.PHONY: all path lua samples $(HOOK_C) clean

path:
	echo $(WAZI_DIR)

$(HOOK_O): $(HOOK_C)
	$(WALI_CC) $(WAZI_CFLAGS) $< -c -o $@

.ONESHELL:
lua: $(HOOK_O)
	make -C lua CC=$(WALI_CC) LD=$(WALI_LD) MYCFLAGS="$(WAZI_CFLAGS)" MYLDFLAGS="$(WAZI_LDFLAGS)" AR="$(WALI_AR) rc" RANLIB=$(WALI_RANLIB) PICOLIBC_ZEPHYR_HOOKS=$<
	wasm2wat --enable-threads lua/lua -o lua/lua.wat

.ONESHELL:
samples: $(HOOK_O)
	make -C samples HOOK=$(HOOK_O)

clean:
	rm -rf samples/wasms
	make -C lua clean
	rm -f $(HOOK)
