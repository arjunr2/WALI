#!/bin/bash

ROOT=..

python3 autogen.py markdown wit
cp autogen/markdown/* $ROOT/docs/
cp autogen/wit/* $ROOT/docs/
