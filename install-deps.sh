#!/bin/bash
if ! command -v apt >/dev/null 2>&1; then
    echo "apt not found; skipping (install equivalent packages for your OS manually)"
    exit 0
fi
apt update
apt install -y make cmake ninja-build gcc wabt libstdc++-12-dev g++ lld
# WAMRC deps
apt install -y build-essential ccache
#g++-multilib libgcc-9-dev lib32gcc-9-dev ccache
