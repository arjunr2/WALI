#!/bin/bash
apt update
apt install -y make cmake ninja-build gcc wabt libstdc++-12-dev g++ lld
# WAMRC deps
apt install build-essential g++-multilib libgcc-9-dev lib32gcc-9-dev ccache
