#!/bin/bash
git submodule update --init wasm-micro-runtime
make iwasm
