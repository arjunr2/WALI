#!/bin/bash

BUILD_FLAG=0

for arg in "$@"
do
    if [[ "$arg" == "--build" ]]; then
        BUILD_FLAG=1
        break
    fi
done

# You can add more script logic below based on the BUILD_FLAG
# For example:

if [ $BUILD_FLAG -ne 0 ]; then
	make uvwasi
fi
ctest --test-dir libs/uvwasi/out/cmake
