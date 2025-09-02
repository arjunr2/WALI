#!/bin/bash
#
# Compile single, standalone C file

# Variables defines the necessary flags/compiler artifacts
source $(dirname $(realpath ${BASH_SOURCE}))/../toolchains/wali.sh

outfile=out.wasm
verbose=""

while getopts "vo:" OPT; do
  case $OPT in
    v) verbose=-v;;
    o) outfile=$OPTARG;;
    *) 
      echo "Incorrect opt provided"
      exit 1 ;;
  esac
done
cfile=${@:$OPTIND:1}

# Compile and link
set -x
$WALI_CC $WALI_COMMON_CFLAGS $WALI_COMMON_LDFLAGS $cfile -o $outfile $verbose
