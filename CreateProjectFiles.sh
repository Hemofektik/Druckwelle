#!/bin/bash

rm -rf build
mkdir build
cd build

cmake -Dprotobuf_BUILD_TESTS=OFF -G "Unix Makefiles" ..

cd ..
gtags -v

