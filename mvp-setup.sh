#!/bin/bash
python3 toolchains/gen_toolchains.py
git submodule update --init wasm-micro-runtime
make iwasm
