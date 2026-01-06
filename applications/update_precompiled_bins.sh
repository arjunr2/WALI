#!/bin/bash

PD=../examples/precompiled

if [ -z "$NOCOMPILE" ]; then
    make sqlite-clean openssh-clean vim-clean
    make -k bash lua memcached mqtt-app sqlite openssh vim
fi

cp bash/build/bash $PD/bash/bash.wasm
cp lua/lua $PD/lua/lua.wasm
cp sqlite/build/sqlite3 $PD/sqlite/sqlite3.wasm

cp memcached/build/memcached $PD/memcached.wasm
cp mqtt-app/mqtt-app $PD/paho-mqtt-app.wasm
cp openssh/build/sshd $PD/sshd.wasm
cp vim/src/vim $PD/vim.wasm