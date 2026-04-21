#!/bin/bash
echo "Hello from WALI bash"
for i in 1 2 3; do
    echo "count: $i"
done
echo "sum: $((1 + 2 + 3))"

# Exercise host syscalls via bash builtins (open/write/read)
tmpfile=/tmp/wali_bash_test_payload
echo "wali-bash-payload" > "$tmpfile"
read -r line < "$tmpfile"
echo "read back: $line"
