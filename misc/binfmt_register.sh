#!/bin/bash
echo -1 > /proc/sys/fs/binfmt_misc/wali-interp
echo -1 > /proc/sys/fs/binfmt_misc/wali-aot
echo ':wali-interp:M::\x00asm::/usr/bin/iwasm-wrapper:' > /proc/sys/fs/binfmt_misc/register
echo ':wali-aot:M::\x00aot::/usr/bin/iwasm-wrapper:' > /proc/sys/fs/binfmt_misc/register
