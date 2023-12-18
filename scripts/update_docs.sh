#!/bin/bash

ROOT=..

python3 autogen.py markdown wit
cp markdown/* $ROOT/docs/
cp wit/* $ROOT/docs/
