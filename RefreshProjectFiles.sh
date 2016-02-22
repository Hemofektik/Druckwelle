#!/bin/bash

cd build

cmake -Dprotobuf_BUILD_TESTS=OFF -G "Unix Makefiles" ..

cd ..
gtags -v

