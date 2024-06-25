include ../wazi_config.mk

SRC_C:=$(wildcard *.c)
SRC_O:=$(SRC_C:.c=.o)

HOOK:=
SAMPLES_OUTDIR:=wasms

.PHONY: all

all: $(SRC_O)

.ONESHELL:
%.o: %.c
	$(WALI_CC) $(WAZI_CFLAGS) $ -c $< -o $(SAMPLES_OUTDIR)/$*.int.wasm
	$(WALI_LD) $(WAZI_LDFLAGS) $(HOOK) $(SAMPLES_OUTDIR)/$*.int.wasm -o $(SAMPLES_OUTDIR)/$*.wasm
	wasm2wat --enable-threads $(SAMPLES_OUTDIR)/$*.wasm -o $(SAMPLES_OUTDIR)/$*.wat
