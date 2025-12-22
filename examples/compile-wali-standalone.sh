#!/bin/bash
#
# Compile single, standalone C file

# Variables defines the necessary flags/compiler artifacts
source $(dirname $(realpath ${BASH_SOURCE}))/../toolchains/wali.sh

outfile=out.wasm
verbose=""
nostartfiles=""
export_startup_cleanup=""

while getopts "vneo:" OPT; do
  case $OPT in
    v) verbose=-v;;
    o) outfile=$OPTARG;;
    n) nostartfiles="-nostartfiles -Wl,--no-entry";;
    e) export_startup_cleanup=" -Wl,--export=__wali_startup -Wl,--export=__wali_cleanup";;
    *) 
      echo "Incorrect opt provided"
      exit 1 ;;
  esac
done
cfile=${@:$OPTIND:1}

# Compile and link
set -x
$WALI_CC $WALI_COMMON_CFLAGS $WALI_COMMON_LDFLAGS $nostartfiles $export_startup_cleanup -o $outfile $verbose $cfile 
