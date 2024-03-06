#!/bin/bash

cp ~/research/libaray/src/backends/build-dir/api/libdpu.so.0.0 ./lib 
cp ~/research/libaray/src/backends/build-dir/api/libdpujni.so.0.0 ./lib
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH;$PWD/lib"